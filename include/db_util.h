//
// Created by thbr on 29.05.2026.
//

#pragma once
#include <string>

#include "libpq-fe.h"

class DbUtil {
public:
    struct DbConnection {
        PGconn *conn;

        explicit DbConnection(const std::string &str);

        ~DbConnection();
    };

    struct ResPtr {
        PGresult *res;

        explicit ResPtr(PGresult *res);

        ~ResPtr();
    };
};
