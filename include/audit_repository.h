//
// Created by thbr on 29.05.2026.
//

#pragma once

#include "libpq-fe.h"

class AuditRepository {
public:
    void Save(PGconn *db_conn, int from_id, int to_id, float amount);

    AuditRepository() = default;
};
