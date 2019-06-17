#pragma once

#include <string>
#include <vector>

#include "interthread_messaging.hpp"

namespace rsg
{
    class TcpSocket;
}

// Contains information for sockets that just connected.
// Once a socket has declared it wants to become an actor socket,
// is is no longer managed by this thread nor uses this data structure.
struct BarelyConnectedSocketInformation
{
    size_t bytes_read = 0; // Stores how many bytes have been read.
    uint32_t content_size = 0; // Size of the following content.
    uint8_t * content_buffer = nullptr; // Content buffer. Initialized once content_size is known.

    ~BarelyConnectedSocketInformation();
};

enum class ServerState
{
    ACCEPTING_NEW_ACTORS,
    WAITING_FOR_ALL_ACTORS_CONNECTION,
    SIMULATION_RUNNING,
    SIMULATION_FINISHED,
    KILLED
};

struct ActorConnection
{
    std::string actor_name = "unset";
    std::string host_name = "unset";
    int actor_id = -1;
    rsg::TcpSocket * socket = nullptr;
    rsg::message_queue * connect_ack = nullptr;
};

std::string server_state_to_string(ServerState state);

int serve(const std::string & platform_file, int server_port, const std::vector<std::string> & simgrid_options);
