#pragma once

#include <string>
#include <thread>
#include <vector>

namespace rsg
{
class TcpSocket;

namespace pb
{
class Decision;
class DecisionAck;
}

//! Manage a connection with the RSG server.
class Connection
{
    friend void connect(int actor_id);
    friend void disconnect();
    friend void * actor_function(void * void_args);

private:
    Connection(const std::string & server_hostname, uint16_t port, int actor_id);
    Connection(const Connection &) = delete;
    ~Connection();

public:
    void send_decision(const rsg::pb::Decision & decision, rsg::pb::DecisionAck & decision_ack);
    void add_child_thread(std::thread * child);

    int actor_id() const;

private:
    int _actor_id = -1;
    TcpSocket * _socket = nullptr;
    std::vector<std::thread*> _children;
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

void connect(int actor_id);
void connect();
void disconnect();

}
