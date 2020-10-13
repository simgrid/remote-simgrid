#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "tcp_listener.hpp"
#include "tcp_socket.hpp"
#include "../assert.hpp"

rsg::TcpListener::TcpListener()
{
    errno = 0;
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    RSG_ENFORCE(_fd != -1, "cannot create socket: %s", strerror(errno));
}

rsg::TcpListener::~TcpListener()
{
    close();
}

void rsg::TcpListener::listen(uint16_t port)
{
    RSG_ASSERT(_fd != -1, "wrong TcpListener::listen call: invalid socket");

    sockaddr_in server_address;
    memset(&server_address, '\0', sizeof(sockaddr_in));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    errno = 0;
    int res = ::bind(_fd, (sockaddr *) &server_address, sizeof(sockaddr_in));
    RSG_ENFORCE(res != -1, "Cannot bind port %hu: %s", port, strerror(errno));

    errno = 0;
    res = ::listen(_fd, 16);
    RSG_ENFORCE(res != -1, "Cannot accept: %s", strerror(errno));
}

rsg::TcpSocket * rsg::TcpListener::accept()
{
    int accepted_fd = ::accept(_fd, nullptr, nullptr);
    RSG_ENFORCE(accepted_fd != -1, "cannot accept: %s\n", strerror(errno));

    return new rsg::TcpSocket(accepted_fd);
}

void rsg::TcpListener::shutdown(bool receive, bool send)
{
    RSG_ASSERT(receive || send, "Wrong TcpListener::shutdown call: at least one of receive or send should be true");

    if (_fd != -1)
    {
        int how;
        if (receive && send) how = SHUT_RDWR;
        else if (receive) how = SHUT_RD;
        else how = SHUT_WR;

        errno = 0;
        int res = ::shutdown(_fd, how);
        RSG_ENFORCE(res != -1, "Cannot shutdown: %s\n", strerror(errno));
    }
}

void rsg::TcpListener::close()
{
    if (_fd != -1)
    {
        errno = 0;
        int res = ::close(_fd);
        _fd = -1;
        RSG_ENFORCE(res != -1, "Cannot close: %s\n", strerror(errno));
    }
}

int rsg::TcpListener::fd() const
{
    return _fd;
}

void rsg::TcpListener::allow_address_reuse()
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::allow_address_reuse call: Invalid socket");

    errno = 0;
    int yes = 1;
    int res = setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&yes), sizeof(int));
    RSG_ENFORCE(res != -1, "Cannot setsockopt SO_REUSEADDR: %s", strerror(errno));
}
