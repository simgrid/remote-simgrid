#include <thread>

#include "actor.hpp"
#include "host.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

static void actor_wrapper(int actor_id, const std::function<void(void *)> & code, void * code_data)
{
    rsg::connection = nullptr;
    RSG_ENFORCE(actor_id != -1, "actor_id should not be -1");
    rsg::connect(actor_id);

    try {
        code(code_data);
    } catch (const rsg::Error & e) {
        printf("%s\n", e.what());
    }

    rsg::disconnect();
}

rsg::Actor::Actor(int id) : _id(id)
{
}

rsg::Actor* rsg::Actor::self()
{
    return new Actor(rsg::connection->actor_id());
}

rsg::Actor* rsg::Actor::create(const std::string & name, const std::shared_ptr<rsg::Host> & host, const std::function<void(void *)>& code, void * code_data)
{
    rsg::pb::Decision decision;
    auto pb_host = new rsg::pb::Host();
    pb_host->set_name(host->get_name());
    auto create_actor = new rsg::pb::Decision_CreateActor();
    create_actor->set_name(name);
    create_actor->set_allocated_host(pb_host);
    decision.set_allocated_actorcreate(create_actor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not create a new Actor");

    // Create a thread for the newly-created actor.
    std::thread * child_thread = new std::thread(actor_wrapper, ack.actorcreate().id(), code, code_data);
    rsg::connection->add_child_thread(child_thread);

    return new Actor(ack.actorcreate().id());
}

std::shared_ptr<rsg::Host> rsg::Actor::get_host()
{
    rsg::pb::Decision decision;
    auto actor = new rsg::pb::Actor();
    actor->set_id(_id);
    decision.set_allocated_actorgethost(actor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Actor(id=%d) does not exist in the simulation", _id);

    return std::shared_ptr<Host>(new Host(ack.actorgethost().name()));
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

int rsg::Actor::get_pid() const
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
