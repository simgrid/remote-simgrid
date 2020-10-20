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

    void wait(std::unique_lock<rsg::Mutex> & lock);
    std::cv_status wait_until(std::unique_lock<rsg::Mutex> & lock, double timeout_time);
    std::cv_status wait_for(std::unique_lock<rsg::Mutex> & lock, double duration);

    void notify_one();
    void notify_all();

private:
    uint64_t _remote_address = 0;

public:
    static ConditionVariablePtr create();
};

}
