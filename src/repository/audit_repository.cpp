//
// Created by thbr on 29.05.2026.
//

#include "audit_repository.h"
#include <stdexcept>
#include "db_util.h"

void AuditRepository::Save(
    PGconn *db_conn,
    const int from_id,
    const int to_id,
    const float amount
) {
    const std::string from_id_str = std::to_string(from_id);
    const std::string to_id_str = std::to_string(to_id);
    const std::string amount_str = std::to_string(amount);

    const char *param_values[3] = {
        from_id_str.c_str(),
        to_id_str.c_str(),
        amount_str.c_str()
    };

    PGresult *res = PQexecParams(
        db_conn,
        "INSERT INTO transactions_history (from_id, to_id, amount) VALUES($1, $2, $3) RETURNING id",
        3,
        nullptr,
        param_values,
        nullptr,
        nullptr,
        0
    );

    const DbUtil::ResPtr guard_insert(res);

    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0) {
        throw std::runtime_error(
            "Ошибка аудита транзакции from: " + from_id_str + ", to: " + to_id_str
            + ". Текст ошибки: " + PQerrorMessage(db_conn)
        );
    }
}
