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

static int retrieve_initial_actor_id_from_env()
{
    const char * initial_actor_id = std::getenv("RSG_INITIAL_ACTOR_ID");
    RSG_ENFORCE(initial_actor_id != nullptr, "Invalid RSG connection initialization: RSG_INITIAL_ACTOR_ID is not set.");
    try
    {
        return std::stoi(std::string(initial_actor_id));
    }
    catch(const std::logic_error & e)
    {
        throw rsg::Error("Invalid RSG connection initialization: RSG_INITIAL_ACTOR_ID ('%s') is not a valid actor id", initial_actor_id);
    }
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
    catch (const std::logic_error & e)
    {
        throw rsg::Error("Invalid RSG connection initialization: RSG_SERVER_PORT ('%s') is not a valid port", server_port);
    }

    actor_id = retrieve_initial_actor_id_from_env();
}

static void retrieve_rsg_think_time_from_env(bool & measure_think_time, double & think_time_double)
{
    // Default behavior. Used if RSG_THINK_TIME is unset.
    measure_think_time = false;
    think_time_double = 0;

    const char * think_time = std::getenv("RSG_THINK_TIME");
    if (think_time == nullptr)
        return;

    if (std::string(think_time) == "AS_MEASURED_BY_CLIENT") {
        measure_think_time = true;
        return;
    }

    // Remaining case is a double value in RSG_THINK_TIME, meaning a constant time increase.
    errno = 0;
    char* endptr = nullptr;
    think_time_double = strtod(think_time, &endptr);
    bool conversion_success = endptr != think_time && *endptr == '\0' && errno != ERANGE;
    RSG_ENFORCE(conversion_success, "RSG_THINK_TIME ('%s') is not a valid double", think_time);
    RSG_ENFORCE(think_time_double >= 0, "RSG_THINK_TIME (%g) should not be negative", think_time_double);
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

    // Retrieve think time mode from environment.
    retrieve_rsg_think_time_from_env(measure_think_time, decision_think_time);
    if (measure_think_time)
        tp_think_time_begin = std::chrono::steady_clock::now();
}

rsg::Connection::Connection(int fd, int actor_id) : _actor_id(actor_id)
{
    // Use an already existing socket.
    _socket = new TcpSocket(fd);
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

    // Wait for the termination of all children threads.
    for (std::thread * child : _children)
    {
        child->join();
    }
}

void rsg::Connection::send_decision(rsg::pb::Decision & decision, rsg::pb::DecisionAck & decision_ack)
{
    RSG_ENFORCE(connection != nullptr, "Invalid librsg call: No connection to rsg server");

    // Add think time to take this decision if requested.
    if (measure_think_time) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - tp_think_time_begin).count();
        decision.set_think_time(elapsed / 1e9);
    } else {
        decision.set_think_time(decision_think_time);
    }

    write_message(decision, *_socket);
    read_message(decision_ack, *_socket);

    if (measure_think_time) {
        tp_think_time_begin = std::chrono::steady_clock::now();
    }
}

void rsg::Connection::add_child_thread(std::thread * child)
{
    RSG_ENFORCE(connection != nullptr, "Invalid librsg call: No connection to rsg server");

    _children.push_back(child);
}

int rsg::Connection::actor_id() const
{
    RSG_ENFORCE(connection != nullptr, "Invalid librsg call: No connection to rsg server");

    return _actor_id;
}

int rsg::Connection::socket_fd() const
{
    RSG_ENFORCE(connection != nullptr, "Invalid librsg call: No connection to rsg server");

    return _socket->fd();
}

void rsg::reuse_connected_socket(int fd)
{
    RSG_ENFORCE(connection == nullptr, "Invalid rsg::reuse_connected_socket() call: Already connected!");

    int actor_id = retrieve_initial_actor_id_from_env();
    connection = new Connection(fd, actor_id);
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
