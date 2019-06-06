#include <SFML/Network.hpp>

#include "simple_commands.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"

#include "rsg.pb.h"

static bool initialize_socket_connection(sf::TcpSocket & socket, const std::string & server_hostname, int server_port)
{
    // Connect to the server.
    sf::Socket::Status status = socket.connect(server_hostname, server_port);
    if (status != sf::Socket::Done)
    {
        printf("Could not connect to rsg server on '%s:%d'\n", server_hostname.c_str(), server_port);
        return false;
    }

    return true;
}

void kill(const std::string & server_hostname, int server_port)
{
    // Connect to the server.
    sf::TcpSocket socket;
    if (!initialize_socket_connection(socket, server_hostname, server_port))
        return;

    // Generate message.
    rsg::Command command;
    command.set_kill(true);

    // Write message on socket.
    write_message(command, socket);
}

void start(const std::string & server_hostname, int server_port)
{
    // Connect to the server.
    sf::TcpSocket socket;
    if (!initialize_socket_connection(socket, server_hostname, server_port))
        return;

    // Generate message.
    rsg::Command command;
    command.set_start(true);

    // Write message on socket.
    write_message(command, socket);
}

void status(const std::string & server_hostname, int server_port)
{
    // Connect to the server.
    sf::TcpSocket socket;
    if (!initialize_socket_connection(socket, server_hostname, server_port))
        return;

    // Generate message.
    rsg::Command command;
    command.set_status(true);

    // Write message on socket.
    write_message(command, socket);
}
