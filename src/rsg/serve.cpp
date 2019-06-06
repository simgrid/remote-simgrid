#include <iostream>
#include <unordered_map>

#include <SFML/Network.hpp>

#include <simgrid/s4u.hpp>
#include <simgrid/actor.h>

#include "interthread_messaging.hpp"
#include "serve.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/protobuf/rsg.pb.h"

struct MaestroArgs
{
    rsg::message_queue * to_command;
};

static void useless_actor()
{
    printf("Hello from useless_actor\n");
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

static bool handle_barely_connected_socket_read(sf::TcpSocket * client_socket,
    BarelyConnectedSocketInformation & info, rsg::Command & command)
{
    size_t bytes_read = 0;
    bool header_read = info.bytes_read >= 4;
    bool header_newly_read = false;
    if (!header_read)
    {
        // Header has not been read completely.
        uint32_t remaining_header_size = 4 - info.bytes_read;
        client_socket->receive(&info.content_size + info.bytes_read, remaining_header_size, bytes_read);
        info.bytes_read += bytes_read;

        RSG_ENFORCE(bytes_read > 0, "Did not read anything from socket (while reading header). Close socket?");

        if (info.bytes_read >= 4)
        {
            // Allocate buffer.
            const uint32_t max_content_size = 16777216;
            RSG_ENFORCE(info.content_size > 0 && info.content_size < max_content_size,
                "Read invalid message content size (%u). Expected it to be in ]0,%u]",
                info.content_size, max_content_size);
            info.content_buffer = (uint8_t *) calloc(info.content_size, sizeof(uint8_t));

            // Flag noise.
            header_newly_read = true;
            header_read = true;
        }
    }

    if (header_read)
    {
        // Message header has already been read. Content may be read now.
        uint32_t remaining_content_size = info.content_size - info.bytes_read + 4;
        client_socket->receive(info.content_buffer + info.bytes_read - 4, remaining_content_size, bytes_read);
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

void handle_command(const rsg::Command & command,
    const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command,
    bool & should_close_socket,
    bool & should_server_stop)
{
    should_close_socket = true;
    switch (command.type_case())
    {
        case rsg::Command::kAddActor:
            printf("Received an ADD_ACTOR command!\n");
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
            break;
        case rsg::Command::TYPE_NOT_SET:
            RSG_ENFORCE(0, "Received a command with an unset type, aborting.");
            break;
    }
}

void serve(const std::string & platform_file, int server_port, const std::vector<std::string> & simgrid_options)
{
    // Run a listening TCP server.
    sf::TcpListener listener;
    sf::Socket::Status status = listener.listen(server_port);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not listen on TCP port %d", server_port);

    // Define inter-thread message queues.
    rsg::message_queue to_command(2);
    rsg::message_queue to_simgrid(2);

    // Put the listening socket and the just accepted socket in a select pool.
    // This enables managing all these sockets from this unique thread.
    sf::SocketSelector connection_selector;
    connection_selector.add(listener);

    // Sockets that just connected and did not send any message yet.
    std::unordered_map<sf::TcpSocket*, BarelyConnectedSocketInformation> barely_connected_sockets;

    for (bool should_server_stop = false; !should_server_stop ; )
    {
        // The timeout defines the latency to terminate this thread when the simulation is finished.
        if (connection_selector.wait(sf::milliseconds(200)))
        {
            // Test whether a new connection can be accepted.
            if (connection_selector.isReady(listener))
            {
                // Listener is ready: there is a pending connection.
                sf::TcpSocket* client = new sf::TcpSocket;
                if (listener.accept(*client) == sf::Socket::Done)
                {
                    client->setBlocking(false);
                    barely_connected_sockets.insert({client, BarelyConnectedSocketInformation()});
                    connection_selector.add(*client);
                }
                else
                {
                    // Error: Just delete the new client socket.
                    delete client;
                }
            }
            else
            {
                // Traverse and test all barely connected sockets.
                for (auto it = barely_connected_sockets.begin(); it != barely_connected_sockets.end(); )
                {
                    sf::TcpSocket * client_socket = it->first;
                    BarelyConnectedSocketInformation & info = it->second;

                    if (!connection_selector.isReady(*client_socket))
                        ++it;
                    else
                    {
                        rsg::Command command;
                        if (handle_barely_connected_socket_read(client_socket, info, command))
                        {
                            bool should_close_socket;
                            handle_command(command, platform_file, simgrid_options, &to_command,
                                should_close_socket, should_server_stop);

                            if (should_close_socket)
                            {
                                connection_selector.remove(*client_socket);
                                it = barely_connected_sockets.erase(it);
                                delete client_socket;
                            }
                        }
                    }
                }
            }
        }
        else // This "else" is entered if no socket message has been received for 500 ms.
        {
            // Has an inter-thread message been received?
            if (!to_command.empty())
            {
                rsg::InterthreadMessage msg;
                to_command.pop(msg);

                if (msg.type == rsg::InterthreadMessageType::SIMULATION_FINISHED)
                    should_server_stop = true;
            }
        }
    }

    listener.close();
}
