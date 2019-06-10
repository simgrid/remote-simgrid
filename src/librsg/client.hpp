#pragma once

namespace rsg
{
    class TcpSocket;

class Client
{
public:
    Client();
    Client(const Client &) = delete;
    ~Client();

private:
    TcpSocket * _socket = nullptr;
};

/* Holds a client instance. MUST be initialized before any librsg call.
   - Either at library load time if RSG_AUTOCONNECT is set.
   - Manually by the user, by calling rsg::connect().
*/
extern thread_local Client* client;

void connect();
void disconnect();

}
