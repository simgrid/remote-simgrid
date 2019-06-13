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
    rsg::pb::Decision decision;
    decision.set_thisactorsleepfor(duration);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}

void rsg::this_actor::sleep_until(double timeout)
{
    rsg::pb::Decision decision;
    decision.set_thisactorsleepuntil(timeout);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}
