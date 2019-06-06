#include "serve.hpp"

#include <iostream>

#include "../kissnet.hpp"
#include "../rsg.pb.h"

void serve(const std::string & platform_file, int server_port)
{
    namespace kn = kissnet;
    kn::socket<kn::protocol::tcp> server(kn::endpoint("0.0.0.0:35000"));
    server.bind();
    server.listen();

    // Accept a new connection.
    auto client = server.accept();
    int client_socket = client.get_underlying_socket();

    // Read a command from the client.
    // Read header.
    uint32_t content_size = 0;
    ssize_t bytes_read = recv(client_socket, &content_size, 4, 0);
    // TODO: ensure all bytes have been read
    // TODO: endianness
    // TODO: check content_size bounds ; last two bytes should be zeros.
    printf("message content is %u-byte long\n", content_size);

    // Read content.
    uint8_t * content = (uint8_t *) calloc(content_size, sizeof(uint8_t));
    bytes_read = recv(client_socket, content, content_size, 0);
    // TODO: ensure all bytes have been read

    // Parse content.
    rsg::Command command;
    bool succeeded = command.ParseFromArray(content, content_size);
    printf("Parsing success: %d\n", (int) succeeded);

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
