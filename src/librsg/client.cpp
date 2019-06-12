#include <cstdlib>

#include "client.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/network/tcp_socket.hpp"

#include "rsg.pb.h"

namespace rsg
{
    thread_local Client* client = nullptr;
}

rsg::Client::Client()
{
    // Read RSG parameters from environment.
    const char * server_hostname = std::getenv("RSG_SERVER_HOSTNAME");
    RSG_ENFORCE(server_hostname != nullptr, "Invalid RSG client initialization: RSG_SERVER_HOSTNAME is not set.");
    std::string hostname(server_hostname);

    const char * server_port = std::getenv("RSG_SERVER_PORT");
    RSG_ENFORCE(server_port != nullptr, "Invalid RSG client initialization: RSG_SERVER_PORT is not set.");
    uint16_t port = 0;
    try
    {
        port = std::stoi(std::string(server_port));
    }
    catch (const rsg::Error & e)
    {
        throw rsg::Error("Invalid RSG client initialization: RSG_SERVER_PORT ('%s') is not a valid port", server_port);
    }

    const char * initial_actor_id = nullptr;
    int actor_id = 0;
    try
    {
        initial_actor_id = std::getenv("RSG_INITIAL_ACTOR_ID");
        RSG_ENFORCE(initial_actor_id != nullptr, "Invalid RSG client initialization: RSG_INITIAL_ACTOR_ID is not set.");
        try
        {
            actor_id = std::stoi(std::string(initial_actor_id));
        }
        catch(const std::exception & e)
        {
            throw rsg::Error("Invalid RSG client initialization: RSG_INITIAL_ACTOR_ID ('%s') is not a valid actor id", initial_actor_id);
        }
    }
    catch (const rsg::Error & e)
    {
        fprintf(stdout, "Killing the server, as this process had no/invalid actor_id.\n");
        fflush(stdout);

        char kill_command[128];
        snprintf(kill_command, 128,
            "rsg kill --reason=\"could not connect: invalid actor_id '%s'\" -h %s -p %s",
            initial_actor_id, server_hostname, server_port);
        system(kill_command);

        throw;
    }

    // Connect to the server.
    _socket = new TcpSocket();
    _socket->connect(server_hostname, port);

    // Generate command.
    rsg::Command command;
    auto actor = new rsg::Actor();
    actor->set_id(actor_id);
    command.set_allocated_connect(actor);

    // Write message on socket.
    write_message(command, *_socket);

    // Read acknowledment.
    rsg::CommandAck command_ack;
    read_message(command_ack, *_socket);

    if (!command_ack.success())
        printf("connect failed\n");
}

rsg::Client::~Client()
{
    delete _socket;
    _socket = nullptr;
}

void rsg::connect()
{
    RSG_ENFORCE(client == nullptr, "Invalid rsg::Client::connect() call: Already connected!");
    client = new Client();
}

static void autoconnect(void) __attribute__((constructor));
void autoconnect(void)
{
    if (std::getenv("RSG_AUTOCONNECT") != nullptr)
        rsg::connect();
}

static void autodisconnect(void) __attribute__((destructor));
void autodisconnect(void)
{
    if (rsg::client != nullptr)
        delete rsg::client;
}
