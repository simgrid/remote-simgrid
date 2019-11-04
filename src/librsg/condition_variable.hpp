#pragma once

#include <cstdint>
#include <vector>
#include <condition_variable>

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
    ~ConditionVariable();

    void wait(rsg::MutexPtr lock);

    std::cv_status wait_until(rsg::MutexPtr lock, double timeout_time);
    std::cv_status wait_for(rsg::MutexPtr lock, double duration);

    void notify_one();
    void notify_all();

private:
    uint64_t _remote_address = 0;

};

}
