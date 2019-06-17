#include "actor.hpp"
#include "host.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"


rsg::Actor::Actor(int id) : _id(id)
{
}

rsg::Actor* rsg::Actor::self()
{
    return new Actor(rsg::connection->actor_id());
}

rsg::Host* rsg::Actor::get_host()
{
    rsg::pb::Decision decision;
    auto actor = new rsg::pb::Actor();
    actor->set_id(_id);
    decision.set_allocated_actorgethost(actor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Actor(id=%d) does not exist in the simulation", _id);

    return new Host(ack.actorgethost().name());
}

std::string rsg::Actor::get_name()
{
    rsg::pb::Decision decision;
    auto actor = new rsg::pb::Actor();
    actor->set_id(_id);
    decision.set_allocated_actorgetname(actor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Actor(id=%d) does not exist in the simulation", _id);
    return ack.actorgetname();
}

int rsg::Actor::get_pid()
{
    return _id;
}



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
    // TODO: propagate failure as exception
}

void rsg::this_actor::sleep_until(double timeout)
{
    rsg::pb::Decision decision;
    decision.set_thisactorsleepuntil(timeout);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    // TODO: propagate failure as exception
}

int rsg::this_actor::get_pid()
{
    return rsg::connection->actor_id();
}
