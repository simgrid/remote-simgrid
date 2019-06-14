#include <cstdlib>

#include "connection.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/network/tcp_socket.hpp"

#include "rsg.pb.h"

namespace rsg
{
    thread_local Connection* connection = nullptr;
}

rsg::Connection::Connection()
{
    // Read RSG parameters from environment.
    const char * server_hostname = std::getenv("RSG_SERVER_HOSTNAME");
    RSG_ENFORCE(server_hostname != nullptr, "Invalid RSG connection initialization: RSG_SERVER_HOSTNAME is not set.");
    std::string hostname(server_hostname);

    const char * server_port = std::getenv("RSG_SERVER_PORT");
    RSG_ENFORCE(server_port != nullptr, "Invalid RSG connection initialization: RSG_SERVER_PORT is not set.");
    uint16_t port = 0;
    try
    {
        port = std::stoi(std::string(server_port));
    }
    catch (const rsg::Error & e)
    {
        throw rsg::Error("Invalid RSG connection initialization: RSG_SERVER_PORT ('%s') is not a valid port", server_port);
    }

    const char * initial_actor_id = nullptr;
    try
    {
        initial_actor_id = std::getenv("RSG_INITIAL_ACTOR_ID");
        RSG_ENFORCE(initial_actor_id != nullptr, "Invalid RSG connection initialization: RSG_INITIAL_ACTOR_ID is not set.");
        try
        {
            _actor_id = std::stoi(std::string(initial_actor_id));
        }
        catch(const std::exception & e)
        {
            throw rsg::Error("Invalid RSG connection initialization: RSG_INITIAL_ACTOR_ID ('%s') is not a valid actor id", initial_actor_id);
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
        int ret = system(kill_command);
        RSG_ENFORCE(ret != -1, "Could not kill server: Could not execute a `rsg` process.");
        RSG_ENFORCE(ret == 0 || ret == 1, "Could not kill server: `rsg kill` returned %d", ret);

        throw;
    }

    // Connect to the server.
    _socket = new TcpSocket();
    _socket->connect(server_hostname, port);
    _socket->disable_nagle_algorithm();

    // Generate command.
    rsg::pb::Command command;
    auto actor = new rsg::pb::Actor();
    actor->set_id(_actor_id);
    command.set_allocated_connect(actor);

    // Write message on socket.
    write_message(command, *_socket);

    // Read acknowledment.
    rsg::pb::CommandAck command_ack;
    read_message(command_ack, *_socket);

    if (!command_ack.success())
        printf("connect failed\n");
}

rsg::Connection::~Connection()
{
    try
    {
        rsg::pb::Decision decision;
        decision.set_quit(true);

        rsg::pb::DecisionAck ack;
        write_message(decision, *_socket);
    }
    catch (const rsg::Error & e)
    {
        printf("Could not tell server that I want to quit: %s\n", e.what());
    }

    delete _socket;
    _socket = nullptr;
}

void rsg::Connection::send_decision(const rsg::pb::Decision & decision, rsg::pb::DecisionAck & decision_ack)
{
    write_message(decision, *_socket);
    read_message(decision_ack, *_socket);
}

int rsg::Connection::actor_id() const
{
    return _actor_id;
}

void rsg::connect()
{
    RSG_ENFORCE(connection == nullptr, "Invalid rsg::connect() call: Already connected!");
    connection = new Connection();
}

static void autoconnect(void) __attribute__((constructor));
void autoconnect(void)
{
    if (std::getenv("RSG_AUTOCONNECT") != nullptr)
        rsg::connect();
}

void rsg::disconnect()
{
    delete rsg::connection;
    rsg::connection = nullptr;
}

static void autodisconnect(void) __attribute__((destructor));
void autodisconnect(void)
{
    rsg::disconnect();
}
