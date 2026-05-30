//
// Created by thbr on 30.05.2026.
//

#include "account_mapper.h"

AccountDto AccountMapper::toDto(
    const int id,
    const std::string &owner_name,
    const float balance
) {
    return AccountDto(
        id,
        owner_name,
        balance
    );
}
