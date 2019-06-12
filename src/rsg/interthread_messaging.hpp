#pragma once

/* Manage communication between the two threads:
   - A Command thread, that manages add-actor/start/status/kill/... requests.
     This thread also keeps a list of open sockets, for clean termination.
   - A SimGrid thread, that runs the simulation.

   Communication is done via lockfree queues.
   This makes it robust and simple (performance is not critical here,
   inter-thread messages will be very rare).
*/

#include <boost/lockfree/queue.hpp>

namespace rsg
{

enum class InterthreadMessageType
{
    UNDEFINED

    // From Command to SimGrid
    ,START_SIMULATION

    // From SimGrid to Command
    ,SIMULATION_FINISHED
    ,SIMULATION_ABORTED

};

// Abstract base class for message content.
struct InterthreadMessageContent
{
    ~InterthreadMessageContent();
};

struct SimulationAbortedContent : public InterthreadMessageContent
{
    ~SimulationAbortedContent();
    std::string abort_reason;
};

struct InterthreadMessage
{
    InterthreadMessageType type = InterthreadMessageType::UNDEFINED;
    InterthreadMessageContent * data = nullptr;
};

// Convenient alias
using message_queue = boost::lockfree::queue<rsg::InterthreadMessage>;

}
