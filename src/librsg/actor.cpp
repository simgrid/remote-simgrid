#include <pthread.h>

#include "actor.hpp"
#include "host.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

static void * actor_function(void * void_args)
{
    int * actor_id = (int*) void_args;
    rsg::connection = nullptr;
    RSG_ENFORCE(*actor_id != -1, "actor_id should not be -1");
    rsg::connect(*actor_id);
    delete actor_id;

    printf("Hello from thread!\n");
    fflush(stdout);

    rsg::disconnect();

    return nullptr;
}

rsg::Actor::Actor(int id) : _id(id)
{
}

rsg::Actor* rsg::Actor::self()
{
    return new Actor(rsg::connection->actor_id());
}

rsg::Actor* rsg::Actor::create(const std::string & name, const rsg::Host * host, const std::function<void()>& code)
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
    printf("Creating a new thread. New actor should have id=%d\n", ack.actorcreate().id());
    fflush(stdout);
    auto new_actor_id = new int;
    *new_actor_id = ack.actorcreate().id();
    pthread_t child_thread;
    int ret = pthread_create(&child_thread, nullptr, actor_function, new_actor_id);
    RSG_ENFORCE(ret == 0, "Could not create thread");
    rsg::connection->add_child_thread(child_thread);

    return new Actor(ack.actorcreate().id());
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
