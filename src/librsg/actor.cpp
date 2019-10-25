#include <thread>

#include <semaphore.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "actor.hpp"
#include "host.hpp"
#include "connection.hpp"
#include "../common/assert.hpp"

#include "rsg.pb.h"

static void actor_wrapper(int actor_id, sem_t * semaphore, const std::function<void(void *)> & code, void * code_arg,
    std::function<void(void *)> before_connect_code, void * before_connect_code_arg,
    std::function<void(void *)> after_connect_code, void * after_connect_code_arg)
{
    try {
        rsg::connection = nullptr;
        RSG_ENFORCE(actor_id != -1, "actor_id should not be -1");
        if (before_connect_code) { before_connect_code(before_connect_code_arg); }
        rsg::connect(actor_id);
        if (after_connect_code) { after_connect_code(after_connect_code_arg); }
        sem_post(semaphore); // Unlocks parent thread.
    } catch(const rsg::Error & e) {
        printf("Error caught while connecting Actor(id=%d) to the server: %s\n", actor_id, e.what());
        sem_post(semaphore); // Unlocks parent thread.
        throw;
    }

    try {
        // Yielding here ensures that this thread is executed in mutual execution with its parent thread.
        rsg::this_actor::yield();
        code(code_arg);
    } catch (const rsg::Error & e) {
        printf("Error caught while running Actor(id=%d): %s\n", actor_id, e.what());
    }

    rsg::disconnect();
}

rsg::Actor::Actor(int id) : _id(id)
{
}

rsg::ActorPtr rsg::Actor::self()
{
    return rsg::ActorPtr(new Actor(rsg::connection->actor_id()));
}

rsg::ActorPtr rsg::Actor::create(const std::string & name, const rsg::HostPtr & host, const std::function<void(void *)>& code, void * code_arg,
    std::function<void(void *)> before_connect_code, void * before_connect_code_arg,
    std::function<void(void *)> after_connect_code, void * after_connect_code_arg)
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

    // This thread should NEVER run concurrently with the new actor's thread. To do so:
    // - A semaphore ensures that this thread waits until the new actor is connected to the server.
    // - Once connected, the new actor yields so it is stopped at least until this actor triggers a simulation event.
    sem_t * semaphore = new sem_t;
    int sem_return = sem_init(semaphore, 0, 0);
    RSG_ENFORCE(sem_return == 0, "Cannot sem_init: %s\n", strerror(errno));

    // Create a thread for the newly-created actor.
    std::thread * child_thread = new std::thread(actor_wrapper, ack.actorcreate().id(), semaphore, code, code_arg,
        before_connect_code, before_connect_code_arg, after_connect_code, after_connect_code_arg);
    rsg::connection->add_child_thread(child_thread);

    // Wait until the child has connected.
    sem_return = sem_wait(semaphore);
    RSG_ENFORCE(sem_return == 0, "Cannot sem_wait: %s\n", strerror(errno));
    sem_return = sem_destroy(semaphore);
    RSG_ENFORCE(sem_return == 0, "Cannot sem_destroy: %s\n", strerror(errno));
    delete semaphore;

    return rsg::ActorPtr(new Actor(ack.actorcreate().id()));
}

int rsg::Actor::fork(const std::string & child_name, const HostPtr & host)
{
    rsg::pb::Decision decision;
    auto pb_host = new rsg::pb::Host();
    pb_host->set_name(host->get_name());
    auto create_actor = new rsg::pb::Decision_CreateActor();
    create_actor->set_name(child_name);
    create_actor->set_allocated_host(pb_host);
    decision.set_allocated_actorcreate(create_actor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Could not create a new Actor");

    /* Call system's fork().
       - On failure, no child is created and -1 is returned to the parent.
       - On success, 0 is returned to the child and the child's pid is returned to the parent.
    */
    int fork_return = ::fork();
    RSG_ENFORCE(fork_return != -1, "Cannot fork: %s\n", strerror(errno));

    if (fork_return == 0) // child process
    {
        rsg::connection = nullptr;
        rsg::connect(ack.actorcreate().id());
        return 0;
    }
    else // parent process
    {
        return ack.actorcreate().id();
    }
}

rsg::HostPtr rsg::Actor::get_host()
{
    rsg::pb::Decision decision;
    auto actor = new rsg::pb::Actor();
    actor->set_id(_id);
    decision.set_allocated_actorgethost(actor);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    RSG_ENFORCE(ack.success(), "Actor(id=%d) does not exist in the simulation", _id);

    return rsg::HostPtr(new Host(ack.actorgethost().name()));
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


void rsg::this_actor::execute(double flop)
{
    rsg::pb::Decision decision;
    decision.set_thisactorexecute(flop);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
    // TODO: propagate failure as exception
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

bool rsg::this_actor::is_maestro()
{
    // Maestro is in the RSG server, not in any client.
    return false;
}

void rsg::this_actor::yield()
{
    rsg::pb::Decision decision;
    decision.set_thisactoryield(true);

    rsg::pb::DecisionAck ack;
    rsg::connection->send_decision(decision, ack);
}
