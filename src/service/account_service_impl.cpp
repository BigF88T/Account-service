//
// Created by thbr on 23.05.2026.
//

#include "account_service_impl.h"

#include "account_service_db_impl.h"

AccountDto AccountServiceImpl::CreateAccount(const std::string &username) {
    AccountDto new_account;
    new_account.id = next_id_++;
    new_account.owner_name = username;
    new_account.balance = 0.0f;

    accounts_[new_account.id] = new_account;

    return new_account;
}

std::optional<AccountDto> AccountServiceImpl::FindAccount(const int id) {
    if (const auto pair = accounts_.find(id); pair != accounts_.end()) {
        return pair->second;
    }
    return std::nullopt;
}

bool AccountServiceImpl::Deposit(const int id, const float amount) {
    if (const auto pair = accounts_.find(id); pair != accounts_.end()) {
        auto& account = pair->second;
        account.balance += amount;

        return true;
    }

    return false;
}

bool AccountServiceImpl::Transfer(const int from_id, const int to_id, const float amount) {

    const auto pair_from = accounts_.find(from_id);
    const auto pair_to = accounts_.find(to_id);

    if (pair_from == accounts_.end() || pair_to == accounts_.end()) return false;

    auto& from_account = pair_from->second;
    auto& to_account = pair_to->second;

    if (from_account.balance - amount >= amount) {
        from_account.balance -= amount;
        to_account.balance += amount;

        return true;
    }

    return false;
}


