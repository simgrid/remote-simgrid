#include <algorithm>

#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/select.h>

#include "selector.hpp"
#include "../assert.hpp"

rsg::Selector::Selector()
{
    clear();
}

rsg::Selector::~Selector()
{
    clear();
}

void rsg::Selector::add(int fd)
{
    FD_SET(fd, &_registered_sockets);
    max_fd = std::max(max_fd, fd);
}

void rsg::Selector::remove(int fd)
{
    FD_CLR(fd, &_registered_sockets);
}

void rsg::Selector::clear()
{
    FD_ZERO(&_registered_sockets);
    FD_ZERO(&_readable_sockets);
}

bool rsg::Selector::wait_any_readable(int timeout_ms)
{
    timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;

    errno = 0;
    _readable_sockets = _registered_sockets;
    int res = ::select(max_fd + 1, &_readable_sockets, nullptr, nullptr, timeout_ms >= 0 ? &timeout : nullptr);
    RSG_ENFORCE(res != -1, "Cannot select: %s", strerror(errno));
    return res > 0;
}

bool rsg::Selector::is_readable(int fd) const
{
    return FD_ISSET(fd, &_readable_sockets);
}

