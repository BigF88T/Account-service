//
// Created by thbr on 23.05.2026.
//

#pragma once

#include "account_service.h"
#include <Crow.h>
#include "transaction_facade.h"

class AccountController {
public:
    explicit AccountController(
        TransactionFacade &transaction_facade,
        AccountService &account_service
    ) : transaction_facade_(transaction_facade), account_service_(account_service) {
    }

    void RegisterRoutes(crow::SimpleApp &app) const;

private:
    TransactionFacade &transaction_facade_;
    AccountService &account_service_;
};
