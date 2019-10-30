#pragma once

#include <cstdint>
#include <vector>

#include "pointers.hpp"

namespace rsg
{

class ConditionVariable
{
private:
    explicit ConditionVariable(uint64_t remote_address);
    ConditionVariable(const ConditionVariable &) = delete;
    ConditionVariable & operator=(const ConditionVariable &) = delete;

public:

private:
    uint64_t _remote_address = 0;
};

}
