#pragma once

#include <string>

#include "interthread_messaging.hpp"

namespace rsg
{
    class TcpSocket;
}
struct ActorConnection;

class Actor
{
public:
    // Initial actors.
    Actor(rsg::TcpSocket * socket, int expected_actor_id, rsg::message_queue * to_command);
    // Dynamically created actors.
    Actor(rsg::message_queue * to_command, rsg::message_queue * connect_ack);

    void operator()();

private:
    rsg::TcpSocket * _socket = nullptr;
    int _id = -1;
    rsg::message_queue * _to_command = nullptr;
    rsg::message_queue * _connect_ack = nullptr;
};

void start_simulation_in_another_thread(const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command,
    const std::unordered_map<int, ActorConnection*> & actor_connections);
