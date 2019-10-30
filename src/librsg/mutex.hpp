#pragma once

#include <memory>
#include <string>

#include "pointers.hpp"

namespace rsg
{
class Mutex
{
private:
    explicit Mutex(uint64_t remote_address);
    Mutex(const Mutex &) = delete;
    Mutex & operator=(const Mutex &) = delete;

public:
    ~Mutex();

    void lock();
    void unlock();
    bool try_lock();

public:
    static MutexPtr create();

private:
    uint64_t _remote_address = 0;
};

}
