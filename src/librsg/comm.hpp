#pragma once

#include <cstdint>

#include "pointers.hpp"

namespace rsg
{

class Comm
{
    friend class Mailbox;
private:
    explicit Comm(uint64_t remote_address, void ** destination_buffer = nullptr);
    Comm(const Comm &) = delete;
    Comm & operator=(const Comm &) = delete;

public:
    ~Comm();

    void start();
    void wait();
    void wait_for(double timeout);
    void cancel();

    bool test();

private:
    uint64_t _remote_address = 0;
    void ** _destination_buffer = nullptr;
};

}
