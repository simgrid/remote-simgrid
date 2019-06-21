#include "engine.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

double rsg::Engine::get_clock()
{
    rsg::pb::Decision decision;
    decision.set_enginegetclock(true);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not get clock");
    return ack.enginegetclock();
}
