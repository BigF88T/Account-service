//
// Created by thbr on 30.05.2026.
//
#pragma once
#include "account_dto.h"

class AccountMapper {
public:
    static AccountDto toDto(
        int id,
        const std::string &owner_name,
        float balance,
        bool is_blocked
    );
};
