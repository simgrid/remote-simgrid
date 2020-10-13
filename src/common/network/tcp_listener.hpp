#pragma once

#include <cstdint>

namespace rsg
{
    class TcpSocket;

// A simple TCP listener socket.
class TcpListener
{
public:
    TcpListener();
    ~TcpListener();
    TcpListener(const TcpListener &) = delete;
    TcpListener(const TcpListener &&) = delete;
    TcpListener & operator=(const TcpListener&) = delete;
    TcpListener & operator=(const TcpListener&&) = delete;

    void listen(uint16_t port);
    TcpSocket * accept();

    void shutdown(bool receive, bool send);
    void close();

    int fd() const;

    void allow_address_reuse();

private:
    int _fd = -1;
};

}
