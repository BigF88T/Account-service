#pragma once

#include <string>
#include <utility>

struct AccountDto {
    explicit AccountDto();

    explicit AccountDto(
        const int id,
        std::string owner_name,
        const float balance
    ) : id(id), owner_name(std::move(owner_name)), balance(balance) {
    }

    int id;
    std::string owner_name;
    float balance;
};
