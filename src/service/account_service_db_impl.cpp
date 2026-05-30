//
// Created by thbr on 24.05.2026.
//
#include "account_service_db_impl.h"
#include <iostream>

#include "audit_repository.h"
#include "db_util.h"
#include "libpq-fe.h"

AccountServiceDbImpl::AccountServiceDbImpl(
    const AccountRepository &account_repository,
    const AuditRepository &audit_repository,
    const std::string &conn_str
) : account_repository_(account_repository), audit_repository_(audit_repository), conn_str_(conn_str) {
}

AccountDto AccountServiceDbImpl::CreateAccount(const std::string &username) {
    const DbUtil::DbConnection db(conn_str_);

    return account_repository_.Save(
        db.conn,
        username
    );
}

std::optional<AccountDto> AccountServiceDbImpl::FindAccount(const int id) {
    const DbUtil::DbConnection db(conn_str_);

    return account_repository_.GetAccountById(
        db.conn,
        id
    );
}

bool AccountServiceDbImpl::Deposit(
    const int id,
    const float amount
) {
    const DbUtil::DbConnection db(conn_str_);

    return account_repository_.DepositById(
        db.conn,
        id,
        amount
    );
}

bool AccountServiceDbImpl::Transfer(
    const int from_id,
    const int to_id,
    const float amount
) {
    const DbUtil::DbConnection db(conn_str_);

    try {
        DbUtil::ResPtr guard_bigun(
            PQexec(
                db.conn,
                "BEGIN"
            )
        );

        const bool isSuccess = account_repository_.TransferByIds(
            db.conn,
            from_id,
            to_id,
            amount
        );

        if (!isSuccess) {
            DbUtil::ResPtr guard_rollback(
                PQexec(
                    db.conn,
                    "ROLLBACK"
                )
            );
            return false;
        }

        audit_repository_.Save(
            db.conn,
            from_id,
            to_id,
            amount
        );

        DbUtil::ResPtr guard_commit(
            PQexec(
                db.conn,
                "COMMIT"
            )
        );

        return true;
    } catch (...) {
        DbUtil::ResPtr guard_rollback(
            PQexec(
                db.conn,
                "ROLLBACK"
            )
        );
        return false;
    }
}
