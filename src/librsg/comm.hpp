#pragma once

namespace rsg
{

class Comm
{
private:
    Comm(uint64_t remote_address);
    Comm(const Comm &) = delete;
    ~Comm();

public:
    void start();
    void wait();
    void wait_for(double timeout);
    void cancel();

    bool test();

private:
    uint64_t _remote_address;
};

}
