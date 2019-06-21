#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "tcp_socket.hpp"
#include "../assert.hpp"

rsg::TcpSocket::TcpSocket()
{
    errno = 0;
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    RSG_ENFORCE(_fd != -1, "cannot create socket: %s", strerror(errno));
}

rsg::TcpSocket::TcpSocket(int fd) : _fd(fd), _connected(true)
{
}


rsg::TcpSocket::~TcpSocket()
{
    close();
}

void rsg::TcpSocket::connect(const std::string & server_address, uint16_t port)
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::connect call: Invalid socket");
    RSG_ASSERT(!_connected, "Wrong TcpSocket::connect call: Already connected");

    struct sockaddr_in server_addr;
    memset(&server_addr, '\0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    errno = 0;
    int res = ::inet_pton(AF_INET, server_address.c_str(), &server_addr.sin_addr);
    RSG_ENFORCE(res != -1, "Cannot connect: %s", strerror(errno));
    RSG_ENFORCE(res != 0, "Cannot connect: '%s' is not a valid address", server_address.c_str());

    errno = 0;
    res = ::connect(_fd, (sockaddr *) &server_addr, sizeof(sockaddr_in));
    RSG_ENFORCE(res != -1, "Cannot connect to (hostname='%s', port=%d): %s",
        server_address.c_str(), port, strerror(errno));
    _connected = true;
}

void rsg::TcpSocket::send_all(const uint8_t * buffer, size_t size)
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::send_all call: Invalid socket");
    RSG_ASSERT(_connected, "Wrong TcpSocket::send_all call: Not connected");

    for (size_t sent = 0; sent < size; )
    {
        errno = 0;
        size_t remaining_size = size - sent;
        ssize_t bytes_sent = ::send(_fd, buffer + sent, remaining_size, 0);
        sent += bytes_sent;

        RSG_ENFORCE(bytes_sent != -1, "Could not send: %s", strerror(errno));
    }
}

void rsg::TcpSocket::recv_all(uint8_t * buffer, size_t size)
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::recv_all call: Invalid socket");
    RSG_ASSERT(_connected, "Wrong TcpSocket::recv_all call: Not connected");

    for (size_t recv = 0; recv < size; )
    {
        errno = 0;
        size_t remaining_size = size - recv;
        ssize_t bytes_recv = ::recv(_fd, buffer + recv, remaining_size, 0);
        recv += bytes_recv;

        RSG_ENFORCE(bytes_recv != -1, "could not recv: %s", strerror(errno));
        if (bytes_recv == 0)
        {
            try
            {
                close();
            }
            catch (const rsg::Error &) {}
            RSG_ENFORCE(0, "Connection lost");
        }
    }
}

void rsg::TcpSocket::recv(uint8_t * buffer, size_t size, size_t & bytes_read)
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::recv call: Invalid socket");
    RSG_ASSERT(_connected, "Wrong TcpSocket::recv call: Not connected");

    bytes_read = 0;

    errno = 0;
    ssize_t bytes_recv = ::recv(_fd, buffer, size, 0);
    RSG_ENFORCE(bytes_recv != -1, "Could not recv: %s", strerror(errno));
    bytes_read = bytes_recv;
    if (bytes_recv == 0)
    {
        try
        {
            close();
        }
        catch (const rsg::Error &) {}
        RSG_ENFORCE(0, "Connection lost");
    }
}

void rsg::TcpSocket::shutdown(bool receive, bool send)
{
    RSG_ASSERT(receive || send, "wrong TcpSocket::shutdown call: at least one of receive or send should be true");

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

void rsg::TcpSocket::close()
{
    _connected = false;

    if (_fd != -1)
    {
        errno = 0;
        int res = ::close(_fd);
        RSG_ENFORCE(res != -1, "Cannot close: %s\n", strerror(errno));
        _fd_before_close = _fd;
        _fd = -1;
    }
}

int rsg::TcpSocket::fd() const
{
    return _fd;
}

int rsg::TcpSocket::fd_before_close() const
{
    return _fd_before_close;
}

bool rsg::TcpSocket::is_connected() const
{
    return _connected;
}

void rsg::TcpSocket::disable_nagle_algorithm()
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::disable_nagle_algorithm call: Invalid socket");

    errno = 0;
    int yes = 1;
    int res = setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(int));
    RSG_ENFORCE(res != -1, "Cannot setsockopt TCP_NODELAY: %s", strerror(errno));
}

void rsg::TcpSocket::set_abortive_termination()
{
    RSG_ASSERT(_fd != -1, "Wrong TcpSocket::set_abortive_termination call: Invalid socket");

    errno = 0;
    linger so_linger;
    so_linger.l_onoff = 1;
    so_linger.l_linger = 0;
    int res = setsockopt(_fd, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&so_linger), sizeof(linger));
    RSG_ENFORCE(res != -1, "Cannot setsockopt SO_LINGER: %s", strerror(errno));
}
