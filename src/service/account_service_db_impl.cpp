//
// Created by thbr on 24.05.2026.
//
#include "account_service_db_impl.h"

#include <iostream>
#include <stdexcept>

#include "libpq-fe.h"

AccountServiceDbImpl::DbConnection::DbConnection(const std::string &str) {
    conn = PQconnectdb(str.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        if (conn) PQfinish(conn);
        throw std::runtime_error("Database connection failed");
    }
}

AccountServiceDbImpl::DbConnection::~DbConnection() {
    if (conn) {
        PQfinish(conn);
    }
}

AccountServiceDbImpl::AccountServiceDbImpl(const std::string &conn_str) : conn_str_(conn_str) {
}

AccountServiceDbImpl::ResPtr::ResPtr(PGresult *res) {
    r = res;
    status_str = "Очистка памяти Heap'а от результатов запроса в БД account-service";
}

AccountServiceDbImpl::ResPtr::~ResPtr() {
    PQclear(r);
    std::cout << status_str;
}

AccountDto AccountServiceDbImpl::CreateAccount(const std::string &username) {
    try {
        const DbConnection db(conn_str_);

        const char *param_values[1] = {username.c_str()};

        PGresult *res = PQexecParams(
            db.conn,
            "INSERT INTO accounts (owner_name) VALUES ($1) RETURNING id, owner_name, balance",
            1, // количество переданных параметров
            nullptr, // тип параметра (если null, то база сама определит)
            param_values,
            nullptr, // длины параметров
            nullptr, // формы (текст)
            0 // результат в тексте
        );

        const ResPtr cleaner(res);

        if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
            throw std::runtime_error("Ошибка при создании пользователя");
        }

        AccountDto new_account;

        new_account.id = std::stoi(PQgetvalue(res, 0, 0));
        new_account.owner_name = PQgetvalue(res, 0, 1);
        new_account.balance = std::stof(PQgetvalue(res, 0, 2));

        return new_account;
    } catch (...) {
        throw std::runtime_error("Ошибка при создании пользователя");
    }
}

std::optional<AccountDto> AccountServiceDbImpl::FindAccount(const int id) {
    try {
        const DbConnection db(conn_str_);

        const std::string id_str = std::to_string(id);
        const char *param_values[1] = {id_str.c_str()};

        PGresult *res = PQexecParams(
            db.conn,
            "SELECT id, owner_name, balance FROM accounts WHERE id = $1",
            1, // количество переданных параметров
            nullptr, // тип параметра (если null, то база сама определит)
            param_values,
            nullptr, // длины параметров
            nullptr, // формы (текст)
            0 // результат в тексте
        );

        const ResPtr cleaner(res);

        if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
            return std::nullopt;
        }

        AccountDto dto;
        dto.id = std::stoi(PQgetvalue(res, 0, 0));
        dto.owner_name = PQgetvalue(res, 0, 1);
        dto.balance = std::stof(PQgetvalue(res, 0, 2));

        return dto;
    } catch (...) {
        return std::nullopt;
    }
}

bool AccountServiceDbImpl::Deposit(const int id, const float amount) {
    try {
        const DbConnection db(conn_str_);
        const std::string id_str = std::to_string(id);
        const std::string amount_str = std::to_string(amount);
        const char *param_values[2] = {id_str.c_str(), amount_str.c_str()};

        PGresult *res = PQexecParams(
            db.conn,
            "UPDATE accounts SET balance = balance + $2 WHERE id = $1",
            2,
            nullptr,
            param_values,
            nullptr,
            nullptr,
            0
        );

        const ResPtr cleaner(res);

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

bool AccountServiceDbImpl::Transfer(const int from_id, const int to_id, const float amount) {
    try {
        const DbConnection db(conn_str_);

        PQexec(db.conn, "BEGIN");

        const std::string from_id_str = std::to_string(from_id);
        const std::string amount_str = std::to_string(amount);
        const char *param_from[2] = {from_id_str.c_str(), amount_str.c_str()};

        PGresult *res_from_transaction = PQexecParams(
            db.conn,
            "UPDATE accounts SET balance = balance - $2 WHERE id = $1 AND balance >= $2",
            2,
            nullptr,
            param_from,
            nullptr,
            nullptr,
            0
        );

        const ResPtr cleaner(res_from_transaction);

        if (PQresultStatus(res_from_transaction) != PGRES_COMMAND_OK || std::stoi(PQcmdTuples(res_from_transaction)) == 0) {
            PQexec(db.conn, "ROLLBACK");
            return false;
        }

        const std::string to_id_str = std::to_string(to_id);
        const char* param_to[2] = {to_id_str.c_str(), amount_str.c_str()};

        PGresult *res_to_transaction = PQexecParams(
            db.conn,
            "UPDATE accounts SET balance = balance + $2 WHERE id = $1",
            2,
            nullptr,
            param_to,
            nullptr,
            nullptr,
            0
        );

        const ResPtr cleaner2(res_to_transaction);

        if (PQresultStatus(res_to_transaction) != PGRES_COMMAND_OK || std::stoi(PQcmdTuples(res_to_transaction)) == 0) {
            PQexec(db.conn, "ROLLBACK");

            return false;
        }

        PQexec(db.conn, "COMMIT");

        return true;
    } catch (...) {
        return false;
    }


}