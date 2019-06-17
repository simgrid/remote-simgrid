#include <time.h>

#include "interthread_messaging.hpp"
#include "../common/assert.hpp"

rsg::InterthreadMessageContent::~InterthreadMessageContent()
{
}

rsg::SimulationAbortedContent::~SimulationAbortedContent()
{
}

rsg::ActorQuitContent::~ActorQuitContent()
{
}

rsg::ActorConnectedContent::~ActorConnectedContent()
{
}

rsg::ActorCreateContent::~ActorCreateContent()
{
}

std::string rsg::interthread_message_type_to_string(const InterthreadMessageType & type)
{
    switch (type)
    {
    case InterthreadMessageType::UNDEFINED:
        return "UNDEFINED";
    case InterthreadMessageType::START_SIMULATION:
        return "START_SIMULATION";
    case InterthreadMessageType::ACTOR_CREATE_ACK:
        return "ACTOR_CREATE_ACK";
    case InterthreadMessageType::ACTOR_CONNECTED:
        return "ACTOR_CONNECTED";
    case InterthreadMessageType::SIMULATION_FINISHED:
        return "SIMULATION_FINISHED";
    case InterthreadMessageType::SIMULATION_ABORTED:
        return "SIMULATION_ABORTED";
    case InterthreadMessageType::ACTOR_QUIT:
        return "ACTOR_QUIT";
    case InterthreadMessageType::ACTOR_CREATE:
        return "ACTOR_CREATE";
    }
    RSG_ENFORCE(0, "Unknown InterthreadMessageType received");
}

void rsg::wait_message_reception(rsg::message_queue * queue)
{
    for (bool stop = false; !stop ;)
    {
        stop = !queue->empty();
        if (!stop)
        {
            timespec delay;
            delay.tv_sec = 0;
            delay.tv_nsec = 1000;
            nanosleep(&delay, nullptr);
        }
    }
}
