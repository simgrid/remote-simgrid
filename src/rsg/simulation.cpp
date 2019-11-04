#include <stdio.h>

#include <simgrid/actor.h>
#include <simgrid/s4u.hpp>

#include "serve.hpp"
#include "simulation.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"

#include "rsg.pb.h"

XBT_LOG_NEW_DEFAULT_CATEGORY(simulation, "The logging channel used in simulation.cpp");

// As I write these lines, this is the SimGrid function to guess the pid of the
// next Actor. As it is quite internal, SimGrid does not provide headers for it.
int SIMIX_process_get_maxpid();

struct MaestroArgs
{
    std::string platform_file;
    rsg::message_queue * to_command;
    std::vector<ActorConnection*> connection_vector;
};

Actor::Actor(RefcountStore * refcount_store, rsg::TcpSocket * socket, int expected_actor_id, rsg::message_queue * to_command) :
    _refcount_store(refcount_store), _socket(socket), _id(expected_actor_id), _to_command(to_command)
{
}

Actor::Actor(RefcountStore * refcount_store, rsg::message_queue * to_command, rsg::message_queue * connect_ack) :
    _refcount_store(refcount_store), _to_command(to_command), _connect_ack(connect_ack)
{
}

static void handle_decision(const rsg::pb::Decision & decision, rsg::pb::DecisionAck & decision_ack,
    rsg::message_queue * to_command, rsg::TcpSocket * socket, RefcountStore * refcount_store,
    bool & send_ack, bool & quit_received)
{
    using namespace simgrid;
    using namespace simgrid::s4u;

    decision_ack.set_success(true);
    send_ack = true;

    switch(decision.type_case())
    {
    case rsg::pb::Decision::kQuit:
    {
        XBT_INFO("Quit decision received. Goodbye.");
        quit_received = true;
        send_ack = false;
    }   break;

    // rsg::Actor methods
    case rsg::pb::Decision::kActorCreate:
    {
        XBT_INFO("Actor::create() received (name='%s', host_name='%s')",
            decision.actorcreate().name().c_str(), decision.actorcreate().host().name().c_str());

        auto host = Host::by_name(decision.actorcreate().host().name());

        // Prepare interthread message queues.
        rsg::message_queue * can_connect_ack = new rsg::message_queue(2);
        rsg::message_queue * connect_ack = new rsg::message_queue(2);

        // Create the new actor.
        auto new_actor = s4u::Actor::create(decision.actorcreate().name().c_str(), host,
            ::Actor(refcount_store, to_command, connect_ack));
        const int new_actor_pid = new_actor->get_pid();

        // Tell the command thread to accept a connection corresponding to this actor.
        rsg::InterthreadMessage msg, ack;
        msg.type = rsg::InterthreadMessageType::ACTOR_CREATE;
        auto content = new rsg::ActorCreateContent();
        content->actor_id = new_actor_pid;
        content->can_connect_ack = can_connect_ack;
        content->connect_ack = connect_ack;
        msg.data = (rsg::InterthreadMessageContent *) content;
        bool could_write = to_command->push(msg);
        RSG_ASSERT(could_write, "Internal interthread messaging error: Actor could not write ACTOR_CREATE");

        // Wait until the command thread knows about the incoming connection.
        wait_message_reception(can_connect_ack);
        bool could_read = can_connect_ack->pop(ack);
        RSG_ASSERT(could_read, "Internal interthread messaging error: Actor could not read ACTOR_CREATE_ACK");
        RSG_ASSERT(ack.type == rsg::InterthreadMessageType::ACTOR_CREATE_ACK,
            "Interthread messaging error: Bad message type (expected ACTOR_CREATE_ACK, got %s)",
            interthread_message_type_to_string(ack.type).c_str());
        delete can_connect_ack;

        // Tell the issuer actor that a new actor has been created, so it can connect to it.
        auto actor = new rsg::pb::Actor();
        actor->set_id(new_actor_pid);
        decision_ack.set_allocated_actorcreate(actor);
        write_message(decision_ack, *socket);
        send_ack = false;
    } break;
    case rsg::pb::Decision::kActorGetHost:
    {
        XBT_INFO("Actor::get_host() received (actor_id=%d)", decision.actorgethost().id());
        auto actor = s4u::Actor::by_pid(decision.actorgethost().id());
        if (actor != nullptr) {
            auto host = new rsg::pb::Host();
            host->set_name(actor->get_host()->get_name());
            decision_ack.set_allocated_actorgethost(host);
        } else {
            decision_ack.set_success(false);
        }
    } break;
    case rsg::pb::Decision::kActorGetName:
    {
        XBT_INFO("Actor::get_name() received (actor_id=%d)", decision.actorgetname().id());
        auto actor = s4u::Actor::by_pid(decision.actorgetname().id());
        if (actor != nullptr) {
            decision_ack.set_actorgetname(actor->get_name());
        } else {
            decision_ack.set_success(false);
        }
    } break;

    // rsg::this_actor methods
    case rsg::pb::Decision::kThisActorExecute:
    {
        XBT_INFO("this_actor::execute received (flop=%g)", decision.thisactorexecute());
        try {
            this_actor::execute(decision.thisactorexecute());
        } catch (const HostFailureException & e) {
            decision_ack.set_success(false);
        }
    } break;
    case rsg::pb::Decision::kThisActorSleepFor:
    {
        XBT_INFO("this_actor::sleep_for received (duration=%g)", decision.thisactorsleepfor());
        try {
            this_actor::sleep_for(decision.thisactorsleepfor());
        } catch (const HostFailureException & e) {
            decision_ack.set_success(false);
        }
    } break;
    case rsg::pb::Decision::kThisActorSleepUntil:
    {
        XBT_INFO("this_actor::sleep_until received (timeout=%g)", decision.thisactorsleepuntil());
        try {
            this_actor::sleep_until(decision.thisactorsleepuntil());
        } catch (const HostFailureException & e) {
            decision_ack.set_success(false);
        }
    } break;
    case rsg::pb::Decision::kThisActorYield:
    {
        XBT_INFO("this_actor::yield received");
        this_actor::yield();
    } break;

    // rsg::Comm methods
    case rsg::pb::Decision::kCommRefcountIncrease:
    {
        XBT_INFO("Comm::refcount_increase received");
        auto comm_it = refcount_store->comms.find(decision.commrefcountincrease().address());
        if (comm_it == refcount_store->comms.end()) {
            decision_ack.set_success(false);
        } else {
            comm_it->second.remote_ref_count++;
        }
    } break;
    case rsg::pb::Decision::kCommRefcountDecrease:
    {
        XBT_INFO("Comm::refcount_decrease received");
        auto comm_it = refcount_store->comms.find(decision.commrefcountdecrease().address());
        if (comm_it == refcount_store->comms.end()) {
            decision_ack.set_success(false);
        } else {
            if (comm_it->second.remote_ref_count == 1) {
                auto comm = comm_it->second.comm;
                intrusive_ptr_release(comm);
                refcount_store->comms.erase(comm_it);
            } else {
                comm_it->second.remote_ref_count--;
            }
        }
    } break;
    case rsg::pb::Decision::kCommStart:
    {
        XBT_INFO("Comm::start received");
        auto comm_it = refcount_store->comms.find(decision.commstart().address());
        if (comm_it == refcount_store->comms.end()) {
            decision_ack.set_success(false);
        } else {
            auto comm = comm_it->second.comm;
            comm->start();
        }
    } break;
    case rsg::pb::Decision::kCommWaitFor:
    {
        XBT_INFO("Comm::wait_for received");
        auto comm_it = refcount_store->comms.find(decision.commwaitfor().comm().address());
        if (comm_it == refcount_store->comms.end()) {
            decision_ack.set_success(false);
        } else {
            auto comm_wait_for = new rsg::pb::DecisionAck_CommWaitFor();
            auto comm = comm_it->second.comm;
            try {
                comm->wait_for(decision.commwaitfor().timeout());
                if (comm_it->second.reception_buffer != nullptr) {
                    auto received_message = (std::string *) *(comm_it->second.reception_buffer);
                    comm_wait_for->set_data(*received_message);
                    delete received_message;
                    delete comm_it->second.reception_buffer;
                    comm_it->second.reception_buffer = nullptr;
                }
                decision_ack.set_allocated_commwaitfor(comm_wait_for);
            } catch (const simgrid::TimeoutException &) {
                comm_wait_for->set_timeoutreached(true);
                decision_ack.set_allocated_commwaitfor(comm_wait_for);
            } catch (const simgrid::CancelException &) {
                decision_ack.set_success(false);
                delete comm_wait_for;
            }
        }
    } break;
    case rsg::pb::Decision::kCommCancel:
    {
        XBT_INFO("Comm::cancel received");
        auto comm_it = refcount_store->comms.find(decision.commcancel().address());
        if (comm_it == refcount_store->comms.end()) {
            decision_ack.set_success(false);
        } else {
            auto comm = comm_it->second.comm;
            comm->start();
        }
    } break;
    case rsg::pb::Decision::kCommTest:
    {
        XBT_INFO("Comm::test received");
        auto comm_it = refcount_store->comms.find(decision.commtest().address());
        if (comm_it == refcount_store->comms.end()) {
            decision_ack.set_success(false);
        } else {
            auto comm = comm_it->second.comm;
            decision_ack.set_commtest(comm->test());
        }
    } break;
    case rsg::pb::Decision::kCommWaitAnyFor:
    {
        XBT_INFO("Comm::wait_any_for received");

        std::vector<CommPtr> comms;
        comms.reserve(decision.commwaitanyfor().comms().size());
        for (auto comm : decision.commwaitanyfor().comms()) {
            auto comm_it = refcount_store->comms.find(comm.address());
            RSG_ENFORCE(comm_it != refcount_store->comms.end(),
                "cannot wait_any_for on unknown Comm pointer %lu", comm.address());
            comms.push_back(comm_it->second.comm);
        }

        auto comm_wait_any_for = new rsg::pb::DecisionAck_CommWaitAnyFor();
        int finished_index = Comm::wait_any_for(&comms, decision.commwaitanyfor().timeout());
        comm_wait_any_for->set_finishedcommindex(finished_index);

        // Transfer received data if needed
        if (finished_index >= 0) { // timeout has NOT been reached
            auto comm_it = refcount_store->comms.find(decision.commwaitanyfor().comms().Get(finished_index).address());
            RSG_ASSERT(comm_it != refcount_store->comms.end(), "Comm memory management inconsistency");
            if (comm_it->second.reception_buffer != nullptr) {
                auto received_message = (std::string *) *(comm_it->second.reception_buffer);
                comm_wait_any_for->set_data(*received_message);
                delete received_message;
                delete comm_it->second.reception_buffer;
                comm_it->second.reception_buffer = nullptr;
            }
        }

        decision_ack.set_allocated_commwaitanyfor(comm_wait_any_for);
    } break;

    // rsg::Engine static methods
    case rsg::pb::Decision::kEngineGetClock:
        decision_ack.set_enginegetclock(s4u::Engine::get_instance()->get_clock());
        break;

    // rsg::Host static methods
    case rsg::pb::Decision::kHostByNameOrNull:
    {
        XBT_INFO("Host::by_name_or_null received (name='%s')", decision.hostbynameornull().c_str());
        auto host = Host::by_name_or_null(decision.hostbynameornull());
        decision_ack.set_success(host != nullptr);
    } break;

    // rsg::Mailbox methods
    case rsg::pb::Decision::kMailboxEmpty:
    {
        XBT_INFO("Mailbox::empty received (mbox_name='%s')", decision.mailboxempty().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxempty().name());
        decision_ack.set_success(mbox->empty());
    } break;
    case rsg::pb::Decision::kMailboxListen:
    {
        XBT_INFO("Mailbox::listen received (mbox_name='%s')", decision.mailboxlisten().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxlisten().name());
        decision_ack.set_success(mbox->listen());
    } break;
    case rsg::pb::Decision::kMailboxReady:
    {
        XBT_INFO("Mailbox::ready received (mbox_name='%s')", decision.mailboxready().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxready().name());
        decision_ack.set_success(mbox->ready());
    } break;
    case rsg::pb::Decision::kMailboxPut:
    {
        XBT_INFO("Mailbox::put received (mbox_name='%s')", decision.mailboxput().mailbox().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxput().mailbox().name());
        const std::string & data = decision.mailboxput().data();

        // Send a std::string, so the receiver knows the size of the real data.
        auto data_to_transfer = new std::string(data.data(), data.size());
        mbox->put((void*) data_to_transfer, decision.mailboxput().simulatedsize());
    } break;
    case rsg::pb::Decision::kMailboxPutAsync:
    {
        XBT_INFO("Mailbox::put_async received (mbox_name='%s')", decision.mailboxputasync().mailbox().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxputasync().mailbox().name());
        const std::string & data = decision.mailboxputasync().data();

        // Send a std::string, so the receiver knows the size of the real data.
        auto data_to_transfer = new std::string(data.data(), data.size());
        auto comm = mbox->put_async((void*) data_to_transfer, decision.mailboxputasync().simulatedsize());
        uint64_t comm_address = (uint64_t) comm.get();

        RSG_ASSERT(refcount_store->comms.find(comm_address) == refcount_store->comms.end(),
            "s4u::Mailbox::put_async returned an already existing Comm");
        RefcountStore::Comm rf_comm;
        rf_comm.comm = comm.get();
        intrusive_ptr_add_ref(rf_comm.comm);
        refcount_store->comms.insert({comm_address, rf_comm});

        auto pb_comm = new rsg::pb::Comm();
        pb_comm->set_address(comm_address);
        decision_ack.set_allocated_mailboxputasync(pb_comm);
    } break;
    case rsg::pb::Decision::kMailboxGet:
    {
        XBT_INFO("Mailbox::get received (mbox_name='%s')", decision.mailboxget().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxget().name());
        std::string * data = (std::string*) mbox->get();
        decision_ack.set_mailboxget(data->data(), data->size());
        write_message(decision_ack, *socket);
        delete data;
        send_ack = false;
    } break;
    case rsg::pb::Decision::kMailboxGetAsync:
    {
        XBT_INFO("Mailbox::get_async received (mbox_name='%s')", decision.mailboxgetasync().name().c_str());
        auto mbox = Mailbox::by_name(decision.mailboxgetasync().name());
        RefcountStore::Comm rf_comm;
        rf_comm.reception_buffer = new void*;
        auto comm = mbox->get_async(rf_comm.reception_buffer);
        uint64_t comm_address = (uint64_t) comm.get();

        RSG_ASSERT(refcount_store->comms.find(comm_address) == refcount_store->comms.end(),
            "s4u::Mailbox::get_async returned an already existing Comm");
        rf_comm.comm = comm.get();
        intrusive_ptr_add_ref(rf_comm.comm);
        refcount_store->comms.insert({comm_address, rf_comm});

        auto pb_comm = new rsg::pb::Comm();
        pb_comm->set_address(comm_address);
        decision_ack.set_allocated_mailboxgetasync(pb_comm);
    } break;

    //rsg::Mutex methods
    case rsg::pb::Decision::kMutexCreate:
    {
        XBT_INFO("Mutex::create received");
        auto mutex = Mutex::create();
        RefcountStore::Mutex rf_mutex;
        uint64_t mutex_address = (uint64_t) mutex.get();
        rf_mutex.mutex = mutex.get();
        intrusive_ptr_add_ref(rf_mutex.mutex);
        refcount_store->mutexs.insert({mutex_address, rf_mutex});

        auto pb_mutex = new rsg::pb::Mutex();
        pb_mutex->set_address(mutex_address);
        decision_ack.set_allocated_mutexcreate(pb_mutex);
    } break;
    case rsg::pb::Decision::kMutexRefcountDecrease:
    {
        XBT_INFO("Mutex::refcount_decrease received");
        auto mutex_it = refcount_store->mutexs.find(decision.mutexrefcountdecrease().address());
        if (mutex_it == refcount_store->mutexs.end()) {
            decision_ack.set_success(false);
        } else {
            if (mutex_it->second.remote_ref_count == 1) {
                auto mutex = mutex_it->second.mutex;
                intrusive_ptr_release(mutex);
                refcount_store->mutexs.erase(mutex_it);
            } else {
                mutex_it->second.remote_ref_count--;
            }
        }
    } break;
    case rsg::pb::Decision::kMutexLock:
    {
        XBT_INFO("Mutex::lock");
        auto mutex_it = refcount_store->mutexs.find(decision.mutexlock().address());
        if (mutex_it == refcount_store->mutexs.end()) {
            decision_ack.set_success(false);
        } else {
            auto mutex = mutex_it->second.mutex;
            mutex->lock();
        }
    } break;
    case rsg::pb::Decision::kMutexUnlock:
    {
        XBT_INFO("Mutex::unlock");
        auto mutex_it = refcount_store->mutexs.find(decision.mutexunlock().address());
        if (mutex_it == refcount_store->mutexs.end()) {
            decision_ack.set_success(false);
        } else {
            auto mutex = mutex_it->second.mutex;
            mutex->unlock();
        }
    } break;
    case rsg::pb::Decision::kMutexTryLock:
    {
        XBT_INFO("Mutex::try_lock");
        auto mutex_it = refcount_store->mutexs.find(decision.mutexunlock().address());
        if (mutex_it == refcount_store->mutexs.end()) {
            decision_ack.set_success(false);
        } else {
            auto mutex = mutex_it->second.mutex;
            decision_ack.set_mutextrylock(mutex->try_lock());
        }
    } break;
    case rsg::pb::Decision::TYPE_NOT_SET:
        RSG_ENFORCE(false, "Received a decision with unset decision type.");
        break;
    }
}

void Actor::operator()()
{
    if (_id != -1)
    {
        // Initial actor. Check that initial state is fine.
        xbt_assert(simgrid::s4u::Actor::self()->get_pid() == _id,
            "Something went wrong while executing initial actors: "
            "My actor id is %ld while I expected it to be %d",
            simgrid::s4u::Actor::self()->get_pid(), _id);
    }
    else
    {
        // Dynamically created actor.
        _id = simgrid::s4u::this_actor::get_pid();

        // Yielding so parent knows our pid.
        simgrid::s4u::this_actor::yield();

        // Wait until the remote actor is connected.
        wait_message_reception(_connect_ack);
        rsg::InterthreadMessage msg;
        bool could_read = _connect_ack->pop(msg);
        RSG_ASSERT(could_read, "Internal interthread messaging error: Actor could not read ACTOR_CONNECTED");
        RSG_ASSERT(msg.type == rsg::InterthreadMessageType::ACTOR_CONNECTED,
            "Interthread messaging error: Bad message type (expected ACTOR_CONNECTED, got %s)",
            interthread_message_type_to_string(msg.type).c_str());
        auto data = (rsg::ActorConnectedContent*) msg.data;
        _socket = data->socket;
        delete data;

        delete _connect_ack;
        _connect_ack = nullptr;
    }

    XBT_DEBUG("Hello.");

    try
    {
        // Actors are simple request-reply loops, as they are controlled remotely.
        for (bool quit_received = false; !quit_received; )
        {
            rsg::pb::Decision decision;
            read_message(decision, *_socket);

            rsg::pb::DecisionAck decision_ack;
            bool send_ack;
            handle_decision(decision, decision_ack, _to_command, _socket, _refcount_store,
                send_ack, quit_received);

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
        XBT_ERROR("%s", e.what());

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

    // Create a unique RefcountStore.
    auto refcount_store = new RefcountStore;

    // Spawn initial actors.
    for (ActorConnection * connection : args->connection_vector)
    {
        auto requested_host = e->host_by_name(connection->host_name);
        simgrid::s4u::Actor::create(connection->actor_name,
            requested_host, Actor(refcount_store, connection->socket, connection->actor_id, to_command));
    }

    // Run the simulation.
    XBT_INFO("Starting the simulation.");
    e->run();
    XBT_INFO("Simulation has finished.");

    // Tell the command thread that the simulation has finished successfully.
    rsg::InterthreadMessage msg;
    msg.type = rsg::InterthreadMessageType::SIMULATION_FINISHED;
    to_command->push(msg);

    delete refcount_store;
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

struct StartSimulationArgs
{
    std::string platform_file;
    std::vector<std::string> simgrid_options;
    rsg::message_queue * to_command;
    std::unordered_map<int, ActorConnection*> actor_connections;
};

static void* start_simulation(void * void_args)
{
    auto my_args = (StartSimulationArgs *) void_args;

    // Run a meastro thread.
    auto args = new MaestroArgs;
    args->platform_file = my_args->platform_file;
    args->to_command = my_args->to_command;
    connections_unorderedmap_to_vector(my_args->actor_connections, args->connection_vector);
    SIMIX_set_maestro(maestro, args);

    /* Prepare argc/argv for Engine creation.
       - Changing the maestro thread is only supported for the thread context factory,
         this is why this factory is forced here.
       - Cleaner methods cannot be be used to set SimGrid options,
         as the context factory is managed at engine creation time.
    */
    int argc = 2 + my_args->simgrid_options.size();
    char * argv[argc];
    argv[0] = strdup("rsg");
    argv[1] = strdup("--cfg=contexts/factory:thread");
    for (int i = 2; i < argc; i++)
        argv[i] = strdup(my_args->simgrid_options[i-2].c_str());
    simgrid::s4u::Engine e(&argc, argv);
    for (int i = 0; i < argc; i++)
        free(argv[i]);
    e.load_platform(my_args->platform_file);

    // Become one of the simulated processes (for a very short time).
    // This is required for now (tested with SimGrid-3.22.2).
    RSG_ASSERT(e.get_host_count() > 0,
        "Error: Provided platform file '%s' contains no host", my_args->platform_file.c_str());
    sg_actor_attach("temporary", nullptr, e.get_all_hosts()[0], nullptr);
    sg_actor_detach();
    delete my_args;
    return nullptr;
}

void start_simulation_in_another_thread(const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command,
    const std::unordered_map<int, ActorConnection*> & actor_connections)
{
    auto args = new StartSimulationArgs();
    args->platform_file = platform_file;
    args->simgrid_options = simgrid_options;
    args->to_command = to_command;
    args->actor_connections = actor_connections;

    pthread_t other_thread;
    pthread_create(&other_thread, nullptr, start_simulation, (void*)args);
}
