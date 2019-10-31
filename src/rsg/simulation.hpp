#pragma once

#include <string>

#include <simgrid/s4u.hpp>

#include "interthread_messaging.hpp"

namespace rsg
{
    class TcpSocket;
}
struct ActorConnection;

// Stores references to refcounted SimGrid objects that have
// remote refcounting management methods.
struct RefcountStore
{
    struct Comm
    {
        simgrid::s4u::Comm * comm = nullptr;
        void ** reception_buffer = nullptr;
        unsigned int remote_ref_count = 1;
    };

    struct Mutex
    {
        simgrid::s4u::Mutex * mutex = nullptr;
        unsigned int remote_ref_count = 1;
    };

    struct ConditionVariable
    {
        simgrid::s4u::ConditionVariable * condition_variable = nullptr;
        unsigned int remote_ref_count = 1;
    };

    std::unordered_map<uint64_t, Comm> comms;
    std::unordered_map<uint64_t, Mutex> mutexes;
    std::unordered_map<uint64_t, ConditionVariable> condition_variables;
};

class Actor
{
public:
    // Initial actors.
    Actor(RefcountStore * refcount_store, rsg::TcpSocket * socket, int expected_actor_id, rsg::message_queue * to_command);
    // Dynamically created actors.
    Actor(RefcountStore * refcount_store, rsg::message_queue * to_command, rsg::message_queue * connect_ack);

    void operator()();

private:
    RefcountStore * _refcount_store = nullptr;
    rsg::TcpSocket * _socket = nullptr;
    int _id = -1;
    rsg::message_queue * _to_command = nullptr;
    rsg::message_queue * _connect_ack = nullptr;
};

void start_simulation_in_another_thread(const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command,
    const std::unordered_map<int, ActorConnection*> & actor_connections);
