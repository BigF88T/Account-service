//
// Created by thbr on 29.05.2026.
//

#include "db_util.h"

#include <stdexcept>

DbUtil::DbConnection::DbConnection(const std::string &str) {
    conn = PQconnectdb(str.c_str());

    if (PQstatus(conn) != CONNECTION_OK) {
        if (conn) PQfinish(conn);
        throw std::runtime_error("Could not connect to database");
    }
}

DbUtil::DbConnection::~DbConnection() {
    if (conn) {
        PQfinish(conn);
    }
}

DbUtil::ResPtr::ResPtr(PGresult *res) {
    this->res = res;
}

DbUtil::ResPtr::~ResPtr() {
    PQclear(res);
}
