#include <iostream>
#include <list>
#include <unordered_map>

#include <signal.h>

#include <simgrid/actor.h>
#include <simgrid/s4u.hpp>

#include "interthread_messaging.hpp"
#include "serve.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/network/selector.hpp"
#include "../common/network/tcp_socket.hpp"
#include "../common/network/tcp_listener.hpp"

#include "rsg.pb.h"

// Open sockets are stored in an (object scope) global variable.
// This is to enable a clean shutdown/close when a signal (SIGINT, SEGV...) is caught.

// Entry point listener socket.
static rsg::TcpListener * listener = nullptr;
// Sockets that just connected and did not send any message yet.
static std::unordered_map<rsg::TcpSocket*, BarelyConnectedSocketInformation> barely_connected_sockets;
// Sockets that have been dropped. Server waits for them to end from the client-side.
static std::list<rsg::TcpSocket *> dropped_sockets;
// Sockets that correspond to simulation actors.
static std::unordered_map<rsg::TcpSocket*, int> actor_sockets;

static void close_open_sockets()
{
    printf("Closing open sockets...");
    fflush(stdout);

    if (listener != nullptr)
    {
        delete listener;
        listener = nullptr;
    }

    for (auto it = barely_connected_sockets.begin(); it != barely_connected_sockets.end(); it++)
    {
        rsg::TcpSocket * socket = it->first;
        socket->shutdown(true, true);
        delete socket;
    }
    barely_connected_sockets.clear();

    for (auto socket : dropped_sockets)
    {
        socket->shutdown(true, true);
        delete socket;
    }
    dropped_sockets.clear();

    for (auto it = actor_sockets.begin(); it != actor_sockets.end(); it++)
    {
        rsg::TcpSocket * socket = it->first;
        socket->shutdown(true, true);
        delete socket;
    }
    actor_sockets.clear();

    printf(" done\n");
}

static void signal_handler(int signal)
{
    switch (signal)
    {
        default:
            printf("Unknown signal (%d) caught\n", signal);
            return;
        case SIGINT:
            printf("SIGINT signal caught!\n");
            break;
        case SIGTERM:
            printf("SIGTERM signal caught!\n");
            break;
        case SIGSEGV:
            printf("Segmentation fault caught!\n");
            break;
    }

    close_open_sockets();
    exit(1);
}

struct MaestroArgs
{
    rsg::message_queue * to_command;
};

static void useless_actor()
{
    printf("Hello from useless_actor\n");
    simgrid::s4u::this_actor::sleep_for(1);
    raise(SIGSEGV);
}

static void maestro(void * void_args)
{
    // Retrieve arguments, use them then clean memory.
    MaestroArgs * args = static_cast<MaestroArgs*>(void_args);
    rsg::message_queue * to_command = args->to_command;
    delete args;

    // Spawn initial actors.
    simgrid::s4u::Engine * e = simgrid::s4u::Engine::get_instance();
    simgrid::s4u::Actor::create("useless", simgrid::s4u::Host::by_name("host0"), useless_actor);

    // Run the simulation.
    printf("Starting SimGrid simulation.\n");
    e->run();
    printf("SimGrid simulation has finished.\n");

    // Tell the command thread that the simulation has finished.
    rsg::InterthreadMessage msg;
    msg.type = rsg::InterthreadMessageType::SIMULATION_FINISHED;
    to_command->push(msg);
}

BarelyConnectedSocketInformation::~BarelyConnectedSocketInformation()
{
    free(content_buffer);
}

static bool handle_barely_connected_socket_read(rsg::TcpSocket * client_socket,
    BarelyConnectedSocketInformation & info, rsg::Command & command)
{
    size_t bytes_read = 0;
    bool header_read = info.bytes_read >= 4;
    bool header_newly_read = false;
    if (!header_read)
    {
        // Header has not been read completely.
        uint32_t remaining_header_size = 4 - info.bytes_read;
        client_socket->recv((uint8_t*) &info.content_size + info.bytes_read, remaining_header_size, bytes_read);
        info.bytes_read += bytes_read;

        RSG_ENFORCE(bytes_read > 0, "Did not read anything from socket (while reading header). Close socket?");

        if (info.bytes_read >= 4)
        {
            // Allocate buffer.
            const uint32_t max_content_size = 16777216;
            RSG_ENFORCE(info.content_size < max_content_size,
                "Read invalid message content size (%u). Expected it to be in [0,%u]",
                info.content_size, max_content_size);
            if (info.content_size > 0)
                info.content_buffer = (uint8_t *) calloc(info.content_size, sizeof(uint8_t));

            // Flag noise.
            header_newly_read = true;
            header_read = true;
        }
    }

    if (header_read)
    {
        // Do not issue another receive if message has 0-byte content.
        if (info.content_size == 0)
            return true;

        // Message header has already been read. Content may be read now.
        uint32_t remaining_content_size = info.content_size - info.bytes_read + 4;
        client_socket->recv(info.content_buffer + info.bytes_read - 4, remaining_content_size, bytes_read);
        info.bytes_read += bytes_read;

        RSG_ENFORCE(bytes_read > 0 || header_newly_read, "Did not read anything from socket (while reading content). Close socket?");

        if (bytes_read == remaining_content_size)
        {
            // The whole message has been read, it can be processed.
            bool deserialize_ok = command.ParseFromArray(info.content_buffer, info.content_size);
            RSG_ENFORCE(deserialize_ok, "Could not deserialize Protobuf Command message");

            return true;
        }
    }

    return false;
}

static void handle_command(const rsg::Command & command,
    rsg::TcpSocket * issuer_socket,
    const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command,
    bool & should_keep_connection,
    bool & should_server_stop)
{
    should_keep_connection = false;
    rsg::CommandAcknowledgment command_ack;
    command_ack.set_success(true);

    switch (command.type_case())
    {
        case rsg::Command::kAddActor:
            printf("Received an ADD_ACTOR command!\n");
            command_ack.set_success(false);
            break;
        case rsg::Command::kStart:
        {
            printf("Received a START command!\n");

            // Run a meastro thread.
            auto args = new MaestroArgs;
            args->to_command = to_command;
            SIMIX_set_maestro(maestro, args);

            // Prepare argc/argv for Engine creation.
            // Changing the maestro thread is only supported for the thread context factory,
            // this is why this factory is forced here.
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
            sg_actor_attach("temporary", nullptr, simgrid::s4u::Host::by_name("host0"), nullptr);
            // Become thread0 again!
            sg_actor_detach();
        } break;
        case rsg::Command::kKill:
            printf("Received a KILL command!\n");
            should_server_stop = true;
            break;
        case rsg::Command::kStatus:
            printf("Received a STATUS command!\n");
            command_ack.set_success(false);
            break;
        case rsg::Command::TYPE_NOT_SET:
            command_ack.set_success(false);
            break;
    }

    write_message(command_ack, *issuer_socket);
}

void serve(const std::string & platform_file, int server_port, const std::vector<std::string> & simgrid_options)
{
    // Run a listening TCP server.
    listener = new rsg::TcpListener();
    try
    {
        listener->listen(server_port);
        printf("Listening on port %d\n", server_port);
    }
    catch (const rsg::Error & e)
    {
        printf("Cannot start server. Reason: %s\n", e.what());
        return;
    }

    // Trap signals to close sockets correctly.
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);

    // Define inter-thread message queues.
    rsg::message_queue to_command(2);
    rsg::message_queue to_simgrid(2);

    rsg::Selector selector;
    selector.add(listener->fd());

    for (bool should_server_stop = false; !should_server_stop ; )
    {
        // The timeout defines the latency to terminate this thread when the simulation is finished.
        if (selector.wait_any_readable(200))
        {
            // Test whether a new connection can be accepted.
            if (selector.is_readable(listener->fd()))
            {
                // Listener is ready: there is a pending connection.
                rsg::TcpSocket* client = listener->accept();
                barely_connected_sockets.insert({client, BarelyConnectedSocketInformation()});
                selector.add(client->fd());
                printf("New client accepted\n");
            }
            else
            {
                bool dropped_a_socket = false;
                // Traverse and test all barely connected sockets.
                for (auto it = barely_connected_sockets.begin(); it != barely_connected_sockets.end(); )
                {
                    rsg::TcpSocket * client_socket = it->first;
                    BarelyConnectedSocketInformation & info = it->second;

                    if (!selector.is_readable(client_socket->fd()))
                        ++it;
                    else
                    {
                        rsg::Command command;
                        if (handle_barely_connected_socket_read(client_socket, info, command))
                        {
                            bool should_keep_connection;
                            handle_command(command, client_socket, platform_file, simgrid_options, &to_command,
                                should_keep_connection, should_server_stop);

                            if (!should_keep_connection)
                            {
                                // Do not close() connection from server first, to avoid server-side TIME-WAIT.
                                // Instead, wait for the client to close it.
                                it = barely_connected_sockets.erase(it);
                                dropped_sockets.push_back(client_socket);
                                dropped_a_socket = true;
                            }
                        }
                    }
                }

                if (!dropped_a_socket)
                {
                    // Traverse and test all dropped sockets
                    for (auto it = dropped_sockets.begin(); it != dropped_sockets.end(); )
                    {
                        rsg::TcpSocket * client_socket = *it;
                        if (!selector.is_readable(client_socket->fd()))
                        {
                            it++;
                        }
                        else
                        {
                            try
                            {
                                uint8_t dropped_buffer[256];
                                size_t bytes_read;
                                client_socket->recv(dropped_buffer, 256, bytes_read);
                                it++;
                            }
                            catch (const rsg::Error & e)
                            {
                                selector.remove(client_socket->fd_before_close());
                                delete client_socket;
                                it = dropped_sockets.erase(it);
                            }
                        }
                    }
                }
            }
        }

        // Has an inter-thread message been received?
        if (!to_command.empty())
        {
            rsg::InterthreadMessage msg;
            to_command.pop(msg);

            if (msg.type == rsg::InterthreadMessageType::SIMULATION_FINISHED)
                should_server_stop = true;
        }
    }

    close_open_sockets();
}
