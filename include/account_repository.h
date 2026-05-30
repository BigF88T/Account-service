//
// Created by thbr on 30.05.2026.
//
#pragma once

#include <optional>
#include "account_dto.h"
#include "libpq-fe.h"

class AccountRepository {
public:
    std::optional<AccountDto> GetAccountById(
        PGconn *db_conn,
        int id
    );

    static AccountDto MapToDto(const PGresult *res);

    AccountDto Save(
        PGconn *db_conn,
        const std::string &username
    );

    bool DepositById(
        PGconn *db_conn,
        int id,
        float amount
    );

    bool TransferByIds(
        PGconn *db_conn,
        int from_id,
        int to_id,
        float amount
    );

    bool IsBlockedById(
        PGconn *db_conn,
        int id
    );
};
