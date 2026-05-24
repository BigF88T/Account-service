#pragma once

#include "account_dto.h"
#include <optional>
#include <string>

class AccountService {
public:

    virtual ~AccountService() = default;

    virtual AccountDto CreateAccount(const std::string& username) = 0;

    virtual std::optional<AccountDto> FindAccount(int id) = 0;

    virtual bool Deposit(int id, float amount) = 0;

    virtual bool Transfer(int from_id, int to_id, float amount) = 0;
};

