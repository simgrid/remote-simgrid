#pragma once

#include <sys/select.h>

namespace rsg
{

class Selector
{
public:
    Selector();
    ~Selector();
    Selector(const Selector &) = delete;
    Selector(const Selector &&) = delete;
    Selector & operator=(const Selector&) = delete;
    Selector & operator=(const Selector&&) = delete;

    void add(int fd);
    void remove(int fd);
    void clear();

    bool wait_any_readable(int timeout_ms);
    bool is_readable(int fd) const;

private:
    fd_set _registered_sockets;
    fd_set _readable_sockets;
    int max_fd = -1;
};

}
