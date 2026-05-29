//
// Created by thbr on 24.05.2026.
//
#pragma once
#include "account_service.h"
#include "libpq-fe.h"
#include "string"
#include "optional"


class AccountServiceDbImpl : public AccountService {
public:

    explicit AccountServiceDbImpl(const std::string& conn_str);

    AccountDto CreateAccount(const std::string& username) override;

    std::optional<AccountDto> FindAccount(int id) override;

    bool Deposit(int id, float amount) override;

    bool Transfer(int from_id, int to_id, float amount) override;

private:
    std::string conn_str_;
};