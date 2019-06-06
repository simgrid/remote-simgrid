#include <SFML/Network.hpp>

#include "status.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/protobuf/rsg.pb.h"

void status(const std::string & server_hostname, int server_port)
{
    // Connect to the server.
    sf::TcpSocket socket;
    sf::Socket::Status status = socket.connect(server_hostname, server_port);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not connect to rsg server on '%s:%d'",
        server_hostname.c_str(), server_port);

    // Generate message.
    rsg::Command command;
    command.set_status(true);

    // Write message on socket.
    write_message(command, socket);
}
