#include "actor.hpp"
#include "connection.hpp"

#include "rsg.pb.h"

bool rsg::this_actor::is_maestro()
{
    // Maestro is in the RSG server, not in any client.
    return false;
}

void rsg::this_actor::sleep_for(double duration)
{
    rsg::Decision decision;
    decision.set_thisactorsleepfor(duration);

    rsg::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}
