//
// Created by thbr on 30.05.2026.
//

#include "account_repository.h"
#include <optional>
#include <stdexcept>

#include "account_mapper.h"
#include "db_util.h"

std::optional<AccountDto> AccountRepository::GetAccountById(
    PGconn *db_conn,
    const int id
) {
    const std::string id_str = std::to_string(id);
    const char *param_values[1] = {id_str.c_str()};

    PGresult *res = PQexecParams(
        db_conn,
        "SELECT id, owner_name, balance, is_blocked FROM accounts WHERE id = $1",
        1,
        // количество переданных параметров
        nullptr,
        // тип параметра (если null, то база сама определит)
        param_values,
        nullptr,
        // длины параметров
        nullptr,
        // формы (текст)
        0 // результат в тексте
    );

    const DbUtil::ResPtr cleaner(res);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        return std::nullopt;
    }

    AccountDto dto = MapToDto(res);

    return dto;
}

AccountDto AccountRepository::Save(PGconn *db_conn, const std::string &username) {
    try {
        const char *param_values[1] = {username.c_str()};

        PGresult *res = PQexecParams(
            db_conn,
            "INSERT INTO accounts (owner_name) VALUES ($1) RETURNING id, owner_name, balance, is_blocked",
            1,
            // количество переданных параметров
            nullptr,
            // тип параметра (если null, то база сама определит)
            param_values,
            nullptr,
            // длины параметров
            nullptr,
            // формы (текст)
            0 // результат в тексте
        );

        const DbUtil::ResPtr cleaner(res);

        if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
            throw std::runtime_error("Ошибка при создании пользователя");
        }

        AccountDto new_account = MapToDto(res);

        return new_account;
    } catch (...) {
        throw std::runtime_error("Ошибка при создании пользователя");
    }
}

bool AccountRepository::DepositById(
    PGconn *db_conn,
    const int id,
    const float amount
) {
    try {
        const std::string id_str = std::to_string(id);
        const std::string amount_str = std::to_string(amount);
        const char *param_values[2] = {id_str.c_str(), amount_str.c_str()};

        PGresult *res = PQexecParams(
            db_conn,
            "UPDATE accounts SET balance = balance + $2 WHERE id = $1",
            2,
            nullptr,
            param_values,
            nullptr,
            nullptr,
            0
        );

        const DbUtil::ResPtr cleaner(res);

        if (PQresultStatus(res) != PGRES_COMMAND_OK) {
            return false;
        }

        //Проверка на наличие строки в БД
        if (std::stoi(PQcmdTuples(res)) == 0) {
            return false;
        }

        return true;
    } catch (...) {
        return false;
    }
}

bool AccountRepository::TransferByIds(
    PGconn *db_conn,
    const int from_id,
    const int to_id,
    const float amount
) {
    try {
        const std::string from_id_str = std::to_string(from_id);
        const std::string amount_str = std::to_string(amount);
        const char *param_from[2] = {
            from_id_str.c_str(),
            amount_str.c_str()
        };

        PGresult *res_from_transaction = PQexecParams(
            db_conn,
            "UPDATE accounts SET balance = balance - $2 WHERE id = $1 AND balance >= $2",
            2,
            nullptr,
            param_from,
            nullptr,
            nullptr,
            0
        );

        const DbUtil::ResPtr guard_update_account1(res_from_transaction);

        if (PQresultStatus(res_from_transaction) != PGRES_COMMAND_OK ||
            std::stoi(PQcmdTuples(res_from_transaction)) == 0) {
            return false;
        }

        const std::string to_id_str = std::to_string(to_id);
        const char *param_to[2] = {
            to_id_str.c_str(),
            amount_str.c_str()
        };

        PGresult *res_to_transaction = PQexecParams(
            db_conn,
            "UPDATE accounts SET balance = balance + $2 WHERE id = $1",
            2,
            nullptr,
            param_to,
            nullptr,
            nullptr,
            0
        );

        const DbUtil::ResPtr guard_update_account2(res_to_transaction);

        if (PQresultStatus(res_to_transaction) != PGRES_COMMAND_OK ||
            std::stoi(PQcmdTuples(res_to_transaction)) == 0) {
            return false;
        }

        return true;
    } catch (...) {
        DbUtil::ResPtr guard_rollback(
            PQexec(
                db_conn,
                "ROLLBACK"
            )
        );
        throw std::runtime_error(
            "Ошибка при выполнении перевода from: " + std::to_string(from_id) +
            ", to: " + std::to_string(to_id) + ". Сообщение об ошибке: " + PQerrorMessage(db_conn)
        );
    }
}

bool AccountRepository::IsBlockedById(
    PGconn *db_conn,
    const int id
) {
    const std::string id_str = std::to_string(id);
    const char *param_values[1] = {id_str.c_str()};

    PGresult *res = PQexecParams(
        db_conn,
        "SELECT is_blocked FROM accounts WHERE id = $1",
        // количество переданных параметров
        1,
        // тип параметра (если null, то база сама определит)
        nullptr,
        param_values,
        // длины параметров
        nullptr,
        // формы (текст)
        nullptr,
        // результат в тексте
        0
    );

    const DbUtil::ResPtr cleaner(res);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        throw std::runtime_error("Не найден пользователь с id: " + id_str);
    }

    const bool is_blocked = PQgetvalue(res, 0, 0)[0] == 't';

    return is_blocked;
}
AccountDto AccountRepository::MapToDto(const PGresult *res) {
    AccountDto new_account = AccountMapper::toDto(
        std::stoi(PQgetvalue(res, 0, 0)),
        PQgetvalue(res, 0, 1),
        std::stof(PQgetvalue(res, 0, 2)),
        PQgetvalue(res, 0, 3)[0] == 't'
    );
    return new_account;
}