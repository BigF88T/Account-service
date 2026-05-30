//
// Created by thbr on 30.05.2026.
//

#include "transaction_facade.h"

bool TransactionFacade::ExecuteTransaction(
    const int from_id,
    const int to_id,
    const float amount
) const {
    const bool isSuccess = account_service_.Transfer(
        from_id,
        to_id,
        amount
    );

    return isSuccess;
}
