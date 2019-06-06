#include <iostream>
#include <unordered_map>

#include <SFML/Network.hpp>

#include <simgrid/s4u.hpp>

#include "interthread_messaging.hpp"
#include "serve.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/protobuf/rsg.pb.h"

struct MaestroArgs
{
    std::string platform_file;
    rsg::message_queue * to_command;
};

static void maestro(void * void_args)
{
    // Retrieve arguments, use them then clean memory.
    MaestroArgs * args = static_cast<MaestroArgs*>(void_args);
    rsg::message_queue * to_command = args->to_command;
    simgrid::s4u::Engine::get_instance()->load_platform(args->platform_file);
    delete args;

    // Run the simulation.
    printf("Starting the SimGrid simulation.\n");
    simgrid::s4u::Engine::get_instance()->run();
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

void serve(const std::string & platform_file, int server_port, std::vector<std::string> & simgrid_options)
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

    for (bool keep_running = true; keep_running ;)
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
                    barely_connected_sockets.emplace(client, BarelyConnectedSocketInformation());
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
                for (auto & it : barely_connected_sockets)
                {
                    sf::TcpSocket * client_socket = it.first;
                    BarelyConnectedSocketInformation & info = it.second;
                    if (connection_selector.isReady(*client_socket))
                    {
                        // Some data is available on the socket.
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
                                rsg::Command command;
                                bool deserialize_ok = command.ParseFromArray(info.content_buffer, info.content_size);
                                RSG_ENFORCE(deserialize_ok, "Could not deserialize Protobuf Command message");

                                bool should_close_socket = true;

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
                                        args->platform_file = platform_file;
                                        args->to_command = &to_command;
                                        SIMIX_set_maestro(maestro, args);

                                        int argc = 1 + simgrid_options.size();
                                        char * argv[argc];
                                        argv[0] = strdup("rsg");
                                        for (int i = 1; i < argc; i++)
                                            argv[i] = strdup(simgrid_options[i].c_str());
                                        simgrid::s4u::Engine e(&argc, argv);
                                        for (int i = 0; i < argc; i++)
                                            free(argv[i]);
                                    } break;
                                    case rsg::Command::kKill:
                                        printf("Received a KILL command!\n");
                                        keep_running = false;
                                        break;
                                    case rsg::Command::kStatus:
                                        printf("Received a STATUS command!\n");
                                        break;
                                    case rsg::Command::TYPE_NOT_SET:
                                        RSG_ENFORCE(0, "Received a command with an unset type, aborting.");
                                        break;
                                }

                                if (should_close_socket)
                                {
                                    connection_selector.remove(*client_socket);
                                    barely_connected_sockets.erase(client_socket);
                                    delete client_socket;
                                }
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // Has an inter-thread message been received?
            if (!to_command.empty())
            {
                rsg::InterthreadMessage msg;
                to_command.pop(msg);

                if (msg.type == rsg::InterthreadMessageType::SIMULATION_FINISHED)
                    keep_running = false;
            }
        }
    }

    listener.close();
}
