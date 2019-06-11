#include <iostream>
#include <list>
#include <unordered_map>

#include <signal.h>

#include "interthread_messaging.hpp"
#include "serve.hpp"
#include "simulation.hpp"
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

BarelyConnectedSocketInformation::~BarelyConnectedSocketInformation()
{
    free(content_buffer);
}

std::string server_state_to_string(ServerState state)
{
    switch(state)
    {
    case ServerState::ACCEPTING_NEW_ACTORS:
        return "ACCEPTING_NEW_ACTORS";
    case ServerState::WAITING_FOR_ALL_ACTORS_CONNECTION:
        return "WAITING_FOR_ALL_ACTORS_CONNECTION";
    case ServerState::SIMULATION_RUNNING:
        return "SIMULATION_RUNNING";
    case ServerState::SIMULATION_FINISHED:
        return "SIMULATION_FINISHED";
    case ServerState::KILLED:
        return "KILLED";
    }
    RSG_ENFORCE(0, "Unknown ServerState received");
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
    ServerState & server_state,
    std::unordered_map<int, ActorConnection*> & actor_connections,
    uint32_t & nb_connected_actors,
    bool & drop_client)
{
    const char * abort_msg = "As this is probably a bug in your experiment setup, the simulation aborts now.";
    rsg::CommandAck command_ack;
    command_ack.set_success(false);
    drop_client = true;

    switch (command.type_case())
    {
        case rsg::Command::kAddActor:
        {
            printf("Received an ADD_ACTOR command! (actor_name=%s, host_name=%s)\n",
                command.addactor().actorname().c_str(), command.addactor().hostname().c_str());
            RSG_ENFORCE(server_state == ServerState::ACCEPTING_NEW_ACTORS ||
                        server_state == ServerState::KILLED,
                "Received an ADD_ACTOR command while the simulation state is '%s'. %s",
                server_state_to_string(server_state).c_str(), abort_msg);

            if (server_state != ServerState::KILLED)
            {
                // Store the actor so a future client can connect later on.
                auto actor_connection = new ActorConnection();
                actor_connection->actor_name = command.addactor().actorname().c_str();
                actor_connection->host_name = command.addactor().hostname().c_str();
                actor_connection->actor_id = actor_connections.size() + 1;
                actor_connections.insert({actor_connection->actor_id, actor_connection});

                // Send the actor identifier back to the client.
                auto actor = new rsg::Actor();
                actor->set_id(actor_connection->actor_id);
                command_ack.set_allocated_actor(actor);
                command_ack.set_success(true);
            }
        } break;
        case rsg::Command::kStart:
            printf("Received a START command!\n");
            RSG_ENFORCE(server_state == ServerState::ACCEPTING_NEW_ACTORS,
                "Received an ADD_ACTOR command while the simulation state is '%s'. %s",
                server_state_to_string(server_state).c_str(), abort_msg);

            if (nb_connected_actors == actor_connections.size())
            {
                server_state = ServerState::SIMULATION_RUNNING;
                start_simulation_in_another_thread(platform_file, simgrid_options, to_command);
            }
            else
            {
                printf("Simulation will start as soon as all registered actors are connected\n");
                server_state = ServerState::WAITING_FOR_ALL_ACTORS_CONNECTION;
            }

            command_ack.set_success(true);
            break;
        case rsg::Command::kKill:
            printf("Received a KILL command! Reason: %s\n", command.kill().c_str());
            server_state = ServerState::KILLED;
            command_ack.set_success(true);
            break;
        case rsg::Command::kStatus:
            printf("Received a STATUS command!\n");
            // TODO: implement me
            break;
        case rsg::Command::kConnect:
        {
            printf("Received a CONNECT command! (actor_id=%d)\n", command.connect().id());
            RSG_ENFORCE(server_state == ServerState::ACCEPTING_NEW_ACTORS ||
                        server_state == ServerState::WAITING_FOR_ALL_ACTORS_CONNECTION ||
                        server_state == ServerState::SIMULATION_RUNNING ||
                        server_state == ServerState::KILLED,
                "Received a CONNECT command while the simulation state is '%s'. %s",
                server_state_to_string(server_state).c_str(), abort_msg);

            if (server_state != ServerState::KILLED)
            {
                // Check actor_id.
                auto it = actor_connections.find(command.connect().id());
                RSG_ENFORCE(it != actor_connections.end(),
                    "Received a CONNECT command from an unknown actor id (%d). %s",
                    command.connect().id(), abort_msg);

                ActorConnection * actor_connection = it->second;
                RSG_ENFORCE(actor_connection->socket == nullptr,
                    "Received a CONNECT command from actor id %d, but a client is already connected to this actor. %s",
                    command.connect().id(), abort_msg);

                // Mark the actor as connected.
                actor_connection->socket = issuer_socket;
                nb_connected_actors++;

                if (server_state == ServerState::WAITING_FOR_ALL_ACTORS_CONNECTION &&
                    nb_connected_actors == actor_connections.size())
                {
                    server_state = ServerState::SIMULATION_RUNNING;
                    start_simulation_in_another_thread(platform_file, simgrid_options, to_command);
                }

                // Contrary to other command connections, this socket should not be dropped now.
                drop_client = false;
                command_ack.set_success(true);
            }
        } break;
        case rsg::Command::TYPE_NOT_SET:
            RSG_ENFORCE(false, "Received a command with unset command type. %s", abort_msg);
            break;
    }

    write_message(command_ack, *issuer_socket);
}

int serve(const std::string & platform_file, int server_port, const std::vector<std::string> & simgrid_options)
{
    // Run a listening TCP server.
    listener = new rsg::TcpListener();
    try
    {
        listener->allow_address_reuse();
        listener->listen(server_port);
        printf("Listening on port %d\n", server_port);
    }
    catch (const rsg::Error & e)
    {
        printf("Cannot start server. Reason: %s\n", e.what());
        return 1;
    }

    ServerState state = ServerState::ACCEPTING_NEW_ACTORS;

    // Trap signals to close sockets correctly.
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGSEGV, signal_handler);

    // Define inter-thread message queues.
    rsg::message_queue to_command(2);
    rsg::message_queue to_simgrid(2);

    rsg::Selector selector;
    selector.add(listener->fd());

    // Store expected client connections (resulting from add-actor commands).
    std::unordered_map<int, ActorConnection*> actor_connections;
    uint32_t nb_connected_actors = 0;

    while (state != ServerState::SIMULATION_FINISHED && state != ServerState::KILLED)
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
                bool client_dropped = false;
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
                            bool drop_client = true;
                            handle_command(command, client_socket, platform_file, simgrid_options,
                                &to_command, state, actor_connections, nb_connected_actors, drop_client);

                            it = barely_connected_sockets.erase(it);
                            if (drop_client)
                            {
                                // Do not close() connection from server first, to limit server-side TIME-WAIT.
                                // Instead, wait for the client to close it.
                                dropped_sockets.push_back(client_socket);
                                client_dropped = true;
                            }
                            else
                            {
                                // Connection has a long lifespan, and will not be managed by the main select().
                                selector.remove(client_socket->fd());
                            }
                        }
                    }
                }

                if (!client_dropped)
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
                state = ServerState::SIMULATION_FINISHED;
        }
    }

    close_open_sockets();
    return 0;
}
