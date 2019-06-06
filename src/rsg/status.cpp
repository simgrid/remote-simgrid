#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#include "status.hpp"
#include "../kissnet.hpp"
#include "../rsg.pb.h"

void status(const std::string & server_hostname, int server_port)
{
    // Connect to the server.
    namespace kn = kissnet;
    kn::tcp_socket sock(kn::endpoint("127.0.0.1:35000"));
    sock.connect();

    // Generate message.
    rsg::Command command;
    command.set_status(true);

    // Write message on socket.
    const uint32_t content_size = command.ByteSize();
    const uint32_t message_size = content_size + 4;
    uint8_t * message_content = (uint8_t *) calloc(message_size, sizeof(uint8_t));
    *message_content = content_size; // TODO: endianness
    command.SerializeToArray(message_content + 4, content_size);

    printf("Message is %u-byte long. Content:", message_size);
    for (int i = 0; i < message_size; i++)
    {
        printf(" %02x", message_content[i]);
    }
    printf("\n");

    printf("Sending message on socket.\n");
    send(sock.get_underlying_socket(), message_content, message_size, 0);

    free(message_content);

    printf("Closing socket\n");
    sock.close();
}
