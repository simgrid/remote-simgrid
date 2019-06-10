#include "simple_commands.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/network/tcp_socket.hpp"

#include "rsg.pb.h"

void add_actor(const std::string & server_hostname, int server_port,
    const std::string & actor_name, const std::string & vhost_name)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::Command command;
        auto add_actor = new(rsg::Command_AddActor);
        add_actor->set_actorname(actor_name);
        add_actor->set_hostname(vhost_name);
        command.set_allocated_addactor(add_actor);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
            printf("add-actor failed\n");
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
    }
}

void kill(const std::string & server_hostname, int server_port)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::Command command;
        command.set_kill(true);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
            printf("kill failed\n");
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
    }
}

void start(const std::string & server_hostname, int server_port)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::Command command;
        command.set_start(true);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
            printf("start failed\n");
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
    }
}

void status(const std::string & server_hostname, int server_port)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::Command command;
        command.set_status(true);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
            printf("status failed\n");
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
    }
}
