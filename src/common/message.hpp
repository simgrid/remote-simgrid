#pragma once
// Convenient functions to write/receive protobuf messages to/from a socket.

/*
    Messages are encoded in binary.
    Formed by a 32-bit header followed by a variable-size content.

    Header contains the content size, as a native-endian 32-bit unsigned integer.
    Last byte should be null, which will allow painless integration
    in languages that do not handle unsigned integers (looking at you java).

    Content is directly serialized/deserialized by protobuf.
*/

#include "assert.hpp"
#include "network/tcp_socket.hpp"

#include <string.h>

template <typename Message>
void write_message(const Message & message, rsg::TcpSocket & socket)
{
    // Generate message buffer (header + content).
    const uint32_t content_size = message.ByteSizeLong();
    const uint32_t message_size = content_size + 4;
    uint8_t message_content[message_size];
    memcpy(message_content, &content_size, 4); // TODO: set endianness
    bool serialize_ok = message.SerializeToArray(message_content + 4, content_size);
    RSG_ENFORCE(serialize_ok, "Could not serialize Protobuf message");

    // Send message on socket.
    socket.send_all(message_content, message_size);
}

template <typename Message>
void read_message(Message & message, rsg::TcpSocket & socket)
{
    // Read header from socket.
    uint32_t content_size = 0;
    const uint32_t max_content_size = 16777216;
    socket.recv_all((uint8_t*) &content_size, 4);
    // TODO: endianness
    RSG_ENFORCE(content_size < max_content_size,
        "Read invalid message content size (%u). Expected it to be in [0,%u]",
        content_size, max_content_size);

    // Read content from socket.
    // Protobuf may encode message of size 0 if they equal default message value.
    if (content_size > 0)
    {
        auto content = new uint8_t[content_size]();
        socket.recv_all(content, content_size);

        // Parse content.
        bool deserialize_ok = message.ParseFromArray(content, content_size);
        delete[] content;
        RSG_ENFORCE(deserialize_ok, "Could not deserialize Protobuf message of content_size=%u", content_size);
    }
}
