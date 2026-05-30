//
// Created by thbr on 30.05.2026.
//
#pragma once
#include "account_service.h"

class TransactionFacade {
public:
    explicit
    TransactionFacade(AccountService &account_service) : account_service_(account_service) {
    }

    bool ExecuteTransaction(
        int from_id,
        int to_id,
        float amount
    ) const;

private:
    AccountService &account_service_;
};
