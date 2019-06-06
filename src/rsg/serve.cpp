#include <iostream>

#include <SFML/Network.hpp>

#include "serve.hpp"
#include "../common/assert.hpp"
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
    // Read header.
    uint32_t content_size = 0;
    size_t bytes_read = 0;
    status = client.receive(&content_size, 4, bytes_read);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not read message header (4 bytes)");
    // TODO: endianness
    // TODO: check content_size bounds ; last two bytes should be zeros.
    printf("message content is %u-byte long\n", content_size);

    // Read content.
    uint8_t * content = (uint8_t *) calloc(content_size, sizeof(uint8_t));
    status = client.receive(content, content_size, bytes_read);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not read message content (%u bytes)", content_size);

    // Parse content.
    rsg::Command command;
    bool succeeded = command.ParseFromArray(content, content_size);
    printf("Parsing success: %d\n", (int) succeeded);

    free(content);

    if (succeeded)
    {
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
                printf("Received an unset command =/\n");
                break;
        }
    }
}
