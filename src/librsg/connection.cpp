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

static void retrieve_rsg_connection_params_from_env(std::string & server_hostname, uint16_t & port, int & actor_id)
{
    const char * c_server_hostname = std::getenv("RSG_SERVER_HOSTNAME");
    RSG_ENFORCE(c_server_hostname != nullptr, "Invalid RSG connection initialization: RSG_SERVER_HOSTNAME is not set.");
    server_hostname = std::string(c_server_hostname);

    const char * server_port = std::getenv("RSG_SERVER_PORT");
    RSG_ENFORCE(server_port != nullptr, "Invalid RSG connection initialization: RSG_SERVER_PORT is not set.");
    try
    {
        port = std::stoi(std::string(server_port));
    }
    catch (const rsg::Error & e)
    {
        throw rsg::Error("Invalid RSG connection initialization: RSG_SERVER_PORT ('%s') is not a valid port", server_port);
    }

    const char * initial_actor_id = std::getenv("RSG_INITIAL_ACTOR_ID");
    RSG_ENFORCE(initial_actor_id != nullptr, "Invalid RSG connection initialization: RSG_INITIAL_ACTOR_ID is not set.");
    try
    {
        actor_id = std::stoi(std::string(initial_actor_id));
    }
    catch(const std::exception & e)
    {
        throw rsg::Error("Invalid RSG connection initialization: RSG_INITIAL_ACTOR_ID ('%s') is not a valid actor id", initial_actor_id);
    }
}


rsg::Connection::Connection(const std::string & server_hostname, uint16_t port, int actor_id) : _actor_id(actor_id)
{
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
    // Wait for the termination of all children threads.
    for (pthread_t child : _children)
    {
        int ret = pthread_join(child, nullptr);
        RSG_ENFORCE(ret == 0, "Error while joining thread");
    }

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

void rsg::Connection::add_child_thread(pthread_t child)
{
    _children.push_back(child);
}

int rsg::Connection::actor_id() const
{
    return _actor_id;
}

void rsg::connect(int actor_id)
{
    RSG_ENFORCE(connection == nullptr, "Invalid rsg::connect() call: Already connected!");

    std::string server_hostname;
    uint16_t port = 0;
    int env_actor_id;

    try {
        retrieve_rsg_connection_params_from_env(server_hostname, port, env_actor_id);
    } catch (const rsg::Error & e) {
        if (strstr(e.what(), "RSG_INITIAL_ACTOR_ID") != nullptr)
        {
            fprintf(stdout, "Killing the server, as this process had no/invalid actor_id.\n");
            fflush(stdout);

            char kill_command[128];
            snprintf(kill_command, 128,
                "rsg kill --reason=\"could not connect: invalid actor_id '%s'\" -h %s -p %d",
                std::getenv("RSG_INITIAL_ACTOR_ID"), server_hostname.c_str(), port);
            int ret = system(kill_command);
            RSG_ENFORCE(ret != -1, "Could not kill server: Could not execute a `rsg` process.");
            RSG_ENFORCE(ret == 0 || ret == 1, "Could not kill server: `rsg kill` returned %d", ret);
        }
        throw;
    }

    if (actor_id == -1)
        actor_id = env_actor_id;

    connection = new Connection(server_hostname, port, actor_id);
}

void rsg::connect()
{
    connect(-1);
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
