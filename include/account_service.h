#pragma once

#include <optional>
#include <string>
#include "account_dto.h"

class AccountService {
public:
    virtual ~AccountService() = default;

    virtual AccountDto CreateAccount(const std::string &username) = 0;

    virtual std::optional<AccountDto> FindAccount(int id) = 0;

    virtual bool Deposit(int id, float amount) = 0;

    virtual bool Transfer(int from_id, int to_id, float amount) = 0;
};
