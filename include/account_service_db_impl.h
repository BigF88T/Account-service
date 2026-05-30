//
// Created by thbr on 24.05.2026.
//
#pragma once
#include "account_repository.h"
#include "account_service.h"
#include "audit_repository.h"
#include "optional"
#include "string"


class AccountServiceDbImpl : public AccountService {
public:
    explicit AccountServiceDbImpl(
        const AccountRepository &account_repository,
        const AuditRepository &audit_repository,
        const std::string &conn_str
    );

    AccountDto CreateAccount(const std::string &username) override;

    std::optional<AccountDto> FindAccount(int id) override;

    bool Deposit(
        int id,
        float amount
    ) override;

    bool Transfer(
        int from_id,
        int to_id,
        float amount
    ) override;

private:
    AccountRepository account_repository_;
    AuditRepository audit_repository_;
    std::string conn_str_;
};
