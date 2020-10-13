#pragma once

/* Manage communication between the two threads:
   - A Command thread, that manages add-actor/start/status/kill/... requests.
     This thread also keeps a list of open sockets, for clean termination.
   - A SimGrid thread, that runs the simulation.

   Communication is done via lockfree queues.
   This makes it robust and simple (performance is not critical here,
   inter-thread messages will be very rare).
*/

#include <string>

#include <boost/lockfree/queue.hpp>

namespace rsg
{
class TcpSocket;

enum class InterthreadMessageType
{
    UNDEFINED

    // From Command to SimGrid
    ,START_SIMULATION
    ,ACTOR_CREATE_ACK
    ,ACTOR_CONNECTED

    // From SimGrid to Command
    ,SIMULATION_FINISHED
    ,SIMULATION_ABORTED
    ,ACTOR_QUIT
    ,ACTOR_CREATE
};

// Abstract base class for message content.
struct InterthreadMessageContent
{
    ~InterthreadMessageContent();
};

struct InterthreadMessage
{
    InterthreadMessageType type = InterthreadMessageType::UNDEFINED;
    InterthreadMessageContent * data = nullptr;
};

// Convenient alias.
using message_queue = boost::lockfree::queue<rsg::InterthreadMessage>;

// Message-specific data structures.
struct SimulationAbortedContent : public InterthreadMessageContent
{
    ~SimulationAbortedContent();
    std::string abort_reason;
};

struct ActorQuitContent : public InterthreadMessageContent
{
    ~ActorQuitContent();
    rsg::TcpSocket * socket_to_drop = nullptr;
};

struct ActorConnectedContent : public InterthreadMessageContent
{
    ~ActorConnectedContent();
    rsg::TcpSocket * socket = nullptr;
};

struct ActorCreateContent : public InterthreadMessageContent
{
    ~ActorCreateContent();
    int actor_id = -1;
    message_queue * can_connect_ack = nullptr;
    message_queue * connect_ack = nullptr;
};

std::string interthread_message_type_to_string(const InterthreadMessageType & type);

void wait_message_reception(message_queue * queue);

}
