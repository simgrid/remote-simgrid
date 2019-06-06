#include <iostream>

#include <SFML/Network.hpp>

#include "serve.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/protobuf/rsg.pb.h"

void serve(const std::string & platform_file, int server_port)
{
    // Run a listening TCP server.
    sf::TcpListener listener;
    sf::Socket::Status status = listener.listen(server_port);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not listen on TCP port %d", server_port);

    // Accept a new connection.
    sf::TcpSocket client;
    status = listener.accept(client);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not accept a new client");

    // Read a command from the client.
    rsg::Command command;
    read_message(command, client);

    switch (command.type_case())
    {
        case rsg::Command::kAddActor:
            printf("Received an ADD_ACTOR command!\n");
            break;
        case rsg::Command::kStart:
            printf("Received a START command!\n");
            break;
        case rsg::Command::kKill:
            printf("Received a KILL command!\n");
            break;
        case rsg::Command::kStatus:
            printf("Received a STATUS command!\n");
            break;
        case rsg::Command::TYPE_NOT_SET:
            RSG_ENFORCE(0, "Received a command with an unset type, aborting.");
            break;
    }
}
