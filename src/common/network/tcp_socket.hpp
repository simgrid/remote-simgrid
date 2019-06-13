#pragma once

#include <cstdint>
#include <string>

namespace rsg
{

// A simple TCP socket.
// - Used to connect to a listening socket.
// - Used to store an accepted socket.
class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int fd);
    ~TcpSocket();

    void connect(const std::string & server_address, uint16_t port);
    void disconnect();

    void send_all(const uint8_t * buffer, size_t size);
    void recv_all(uint8_t * buffer, size_t size);
    void recv(uint8_t * buffer, size_t size, size_t & bytes_read);

    void shutdown(bool receive, bool send);
    void close();

    int fd() const;
    int fd_before_close() const;
    bool is_connected() const;

    void disable_nagle_algorithm();
    void set_abortive_termination();

private:
    int _fd = -1;
    int _fd_before_close = -1;
    bool _connected = false;
};

}
