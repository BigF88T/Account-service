#pragma once

#include <unordered_map>

#include "account_service.h"


class AccountServiceImpl : public AccountService {
public:
    AccountDto CreateAccount(const std::string& username) override;

    std::optional<AccountDto> FindAccount(int id) override;

    bool Deposit(int id, float amount) override;

    bool Transfer(int from_id, int to_id, float amount) override;

private:
    std::unordered_map<int, AccountDto> accounts_;
    int next_id_ = 1;
};