#pragma once

#include <string>
#include <utility>

struct AccountDto {
    explicit AccountDto() = default;

    explicit AccountDto(
        const int id,
        std::string owner_name,
        const float balance,
        const bool is_blocked
    ) : id(id), owner_name(std::move(owner_name)), balance(balance), is_blocked(is_blocked) {
    }

    int id;
    std::string owner_name;
    float balance;
    bool is_blocked;
};
