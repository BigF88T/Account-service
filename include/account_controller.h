//
// Created by thbr on 23.05.2026.
//

#pragma once

#include "Crow.h"
#include "account_service.h"

class AccountController {
public:
    explicit AccountController(AccountService& account_service) : account_service_(account_service) {}

    void RegisterRoutes(crow::SimpleApp& app);

private:
    AccountService& account_service_;
};
