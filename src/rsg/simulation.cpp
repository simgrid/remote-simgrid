#include <stdio.h>

#include <simgrid/actor.h>
#include <simgrid/s4u.hpp>

#include "serve.hpp"
#include "simulation.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"

#include "rsg.pb.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(simulation, "The logging channel used in simulation.cpp");

struct MaestroArgs
{
    std::string platform_file;
    rsg::message_queue * to_command;
    std::vector<ActorConnection*> connection_vector;
};

Actor::Actor(rsg::TcpSocket * socket, int expected_actor_id, rsg::message_queue * to_command) :
    _socket(socket), _id(expected_actor_id), _to_command(to_command)
{
}

static void handle_decision(const rsg::Decision & decision, rsg::DecisionAck & decision_ack,
    bool & send_ack, bool & quit_received)
{
    using namespace simgrid;
    using namespace simgrid::s4u;

    decision_ack.set_success(true);
    send_ack = true;

    switch(decision.type_case())
    {
    case rsg::Decision::kQuit:
    {
        XBT_INFO("Quit decision received. Goodbye.");
        quit_received = true;
        send_ack = false;
    }   break;
    case rsg::Decision::kThisActorSleepFor:
    {
        XBT_INFO("sleep_for received (duration=%g)", decision.thisactorsleepfor());
        try {
            this_actor::sleep_for(decision.thisactorsleepfor());
        } catch (const HostFailureException & e) {
            decision_ack.set_success(false);
        }
    } break;
    case rsg::Decision::kThisActorSleepUntil:
    {
        XBT_INFO("sleep_until received (timeout=%g)", decision.thisactorsleepuntil());
        try {
            this_actor::sleep_until(decision.thisactorsleepuntil());
        } catch (const HostFailureException & e) {
            decision_ack.set_success(false);
        }
    }   break;
    case rsg::Decision::TYPE_NOT_SET:
        RSG_ENFORCE(false, "Received a decision with unset decision type.");
        break;
    }
}

void Actor::operator()()
{
    // Check that initial state is fine.
    xbt_assert(simgrid::s4u::Actor::self()->get_pid() == _id,
        "Something went wrong while executing initial actors: "
        "My actor id is %ld while I expected it to be %d",
        simgrid::s4u::Actor::self()->get_pid(), _id);

    XBT_DEBUG("Hello.");

    try
    {
        // Actors are simple request-reply loops, as they are controlled remotely.
        for (bool quit_received = false; !quit_received; )
        {
            rsg::Decision decision;
            read_message(decision, *_socket);

            rsg::DecisionAck decision_ack;
            bool send_ack;
            handle_decision(decision, decision_ack, send_ack, quit_received);

            if (send_ack)
            {
                write_message(decision_ack, *_socket);
            }
        }

        // Tell the command thread to drop the socket associated with this actor.
        rsg::InterthreadMessage msg;
        msg.type = rsg::InterthreadMessageType::ACTOR_QUIT;
        auto content = new rsg::ActorQuitContent();
        content->socket_to_drop = _socket;
        msg.data = (rsg::InterthreadMessageContent *) content;
        _to_command->push(msg);
    }
    catch (const rsg::Error & e)
    {
        // Tell the command thread that an actor failed.
        rsg::InterthreadMessage msg;
        msg.type = rsg::InterthreadMessageType::SIMULATION_ABORTED;
        auto content = new rsg::SimulationAbortedContent();

        char reason[256];
        snprintf(reason, 256, "Problem detected with remote actor_id=%d: %s", _id, e.what());
        content->abort_reason = std::string(reason);
        msg.data = (rsg::InterthreadMessageContent *) content;
        _to_command->push(msg);
    }
}

static void maestro(void * void_args)
{
    // Retrieve arguments
    MaestroArgs * args = static_cast<MaestroArgs*>(void_args);
    rsg::message_queue * to_command = args->to_command;

    // Prepare simulation initial state
    simgrid::s4u::Engine * e = simgrid::s4u::Engine::get_instance();

    // Check that all requested actors are valid.
    bool abort_launch = false;
    char error_message[256];
    for (ActorConnection * connection : args->connection_vector)
    {
        auto requested_host = e->host_by_name_or_null(connection->host_name);
        if (requested_host == nullptr)
        {
            abort_launch = true;
            snprintf(error_message, 256,
                "Invalid initial state: actor_id=%d requested host='%s', "
                "but this host does not exist in the provided platform file '%s'",
                connection->actor_id, connection->host_name.c_str(),
                args->platform_file.c_str());
            break;
        }
    }

    if (abort_launch)
    {
        // Run an empty simulation. This is done to terminate cleanly, notably to close sockets
        e->run();

        // Tell the command thread that the simulation could not be launched.
        rsg::InterthreadMessage msg;
        msg.type = rsg::InterthreadMessageType::SIMULATION_ABORTED;
        auto content = new rsg::SimulationAbortedContent();
        content->abort_reason = std::string(error_message);
        msg.data = (rsg::InterthreadMessageContent *) content;
        to_command->push(msg);

        delete args;
        return;
    }

    // Spawn initial actors.
    for (ActorConnection * connection : args->connection_vector)
    {
        auto requested_host = e->host_by_name(connection->host_name);
        simgrid::s4u::Actor::create(connection->actor_name,
            requested_host, Actor(connection->socket, connection->actor_id, to_command));
    }

    // Run the simulation.
    XBT_INFO("Starting the simulation.");
    e->run();
    XBT_INFO("Simulation has finished.");

    // Tell the command thread that the simulation has finished successfully.
    rsg::InterthreadMessage msg;
    msg.type = rsg::InterthreadMessageType::SIMULATION_FINISHED;
    to_command->push(msg);

    delete args;
}

static void connections_unorderedmap_to_vector(
    const std::unordered_map<int, ActorConnection*> & connection_map,
    std::vector<ActorConnection *> & connection_vector)
{
    connection_vector.clear();
    if (connection_map.empty())
        return;

    // Populate output vector.
    for (auto it = connection_map.begin(); it != connection_map.end(); ++it)
    {
        connection_vector.push_back(it->second);
    }

    // Sort output vector by ascending actor id.
    std::sort(connection_vector.begin(), connection_vector.end(),
        [](const ActorConnection * a, const ActorConnection * b) -> bool
        {
            return a->actor_id < b->actor_id;
        }
    );

    // Make sure that actor ids are exactly {2, 3, ..., n, n+1}
    RSG_ENFORCE(connection_vector[0]->actor_id == 2,
        "Minimal initial actor_id value is %d, while I expected %d",
        connection_vector[0]->actor_id, 2);
    for (size_t i = 1; i < connection_vector.size(); i++)
    {
        RSG_ENFORCE(connection_vector[i-1]->actor_id + 1 == connection_vector[i]->actor_id,
            "Initial actor ids are not consecutive: %d is just after %d",
            connection_vector[i]->actor_id, connection_vector[i-1]->actor_id);
    }

    RSG_ASSERT(connection_vector.size() == connection_map.size(),
        "fix connections_unorderedmap_to_vector: bad output vector size");
    // Yay, all checks passed!
}

void start_simulation_in_another_thread(const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command,
    const std::unordered_map<int, ActorConnection*> & actor_connections)
{
    // Run a meastro thread.
    auto args = new MaestroArgs;
    args->platform_file = platform_file;
    args->to_command = to_command;
    connections_unorderedmap_to_vector(actor_connections, args->connection_vector);
    SIMIX_set_maestro(maestro, args);

    /* Prepare argc/argv for Engine creation.
       - Changing the maestro thread is only supported for the thread context factory,
         this is why this factory is forced here.
       - Cleaner methods cannot be be used to set SimGrid options,
         as the context factory is managed at engine creation time.
    */
    int argc = 2 + simgrid_options.size();
    char * argv[argc];
    argv[0] = strdup("rsg");
    argv[1] = strdup("--cfg=contexts/factory:thread");
    for (int i = 2; i < argc; i++)
        argv[i] = strdup(simgrid_options[i-2].c_str());
    simgrid::s4u::Engine e(&argc, argv);
    for (int i = 0; i < argc; i++)
        free(argv[i]);
    e.load_platform(platform_file);

    // Become one of the simulated processes (for a very short time).
    // This is required for now (tested with SimGrid-3.22.2).
    RSG_ASSERT(e.get_host_count() > 0,
        "Error: Provided platform file '%s' contains no host", platform_file.c_str());
    sg_actor_attach("temporary", nullptr, e.get_all_hosts()[0], nullptr);

    // Become thread0 again!
    sg_actor_detach();
}