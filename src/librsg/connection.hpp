#pragma once

namespace rsg
{
class TcpSocket;
class Decision;
class DecisionAck;

//! Manage a connection with the RSG server.
class Connection
{
    friend void connect();
    friend void disconnect();

private:
    Connection();
    Connection(const Connection &) = delete;
    ~Connection();

public:
    void send_decision(const rsg::Decision & decision, rsg::DecisionAck & decision_ack);

private:
    TcpSocket * _socket = nullptr;
};

/* Holds a client instance.
   MUST be initialized before any librsg call is done in the current thread.

   The main thread is either initialized at library load time if RSG_AUTOCONNECT is set,
   or should be done manually by the user by calling rsg::connect() before doing any other librsg call.

   Other threads created by librsg (by calling rsg::Actor::create) should be
   initialized automatically.

   Other user-created threads should not directly call RSG functions.
*/
extern thread_local Connection* connection;

void connect();
void disconnect();

}
