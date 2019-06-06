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

#include <SFML/Network.hpp>

#include "assert.hpp"

template <typename Message>
void write_message(const Message & message, sf::TcpSocket & socket)
{
    // Generate message buffer (header + content).
    const uint32_t content_size = message.ByteSize();
    const uint32_t message_size = content_size + 4;
    uint8_t * message_content = (uint8_t *) calloc(message_size, sizeof(uint8_t));
    *message_content = content_size; // TODO: set endianness
    bool serialize_ok = message.SerializeToArray(message_content + 4, content_size);
    RSG_ENFORCE(serialize_ok, "Could not serialize Protobuf message");

    // Debug printing. Might be useful later on.
    printf("Message is %u-byte long. Content:", message_size);
    for (int i = 0; i < message_size; i++)
    {
        printf(" %02x", message_content[i]);
    }
    printf("\n");

    // Send message on socket.
    sf::Socket::Status status = socket.send(message_content, message_size);
    free(message_content);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not send message on socket");
}

template <typename Message>
void read_message(Message & message, sf::TcpSocket & socket)
{
    // Read header from socket.
    uint32_t content_size = 0;
    const uint32_t max_content_size = 16777216;
    size_t bytes_read = 0;
    sf::Socket::Status status = socket.receive(&content_size, 4, bytes_read);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not read message header (4 bytes)");
    RSG_ENFORCE(bytes_read == 4, "Could not read message header (4 bytes)");
    // TODO: endianness
    RSG_ENFORCE(content_size > 0 && content_size < max_content_size,
        "Read invalid message content size (%u). Expected it to be in ]0,%u]",
        content_size, max_content_size);

    // Read content from socket.
    uint8_t * content = (uint8_t *) calloc(content_size, sizeof(uint8_t));
    status = socket.receive(content, content_size, bytes_read);
    RSG_ENFORCE(status == sf::Socket::Done, "Could not read message content (%u bytes)", content_size);
    RSG_ENFORCE(bytes_read == content_size, "Could not read message header (%u bytes)", content_size);

    // Debug printing. Might be useful later on.
    printf("Received message content is %u-byte long:", content_size);
    for (int i = 0; i < content_size; i++)
    {
        printf(" %02x", content[i]);
    }
    printf("\n");

    // Parse content.
    bool deserialize_ok = message.ParseFromArray(content, content_size);
    RSG_ENFORCE(deserialize_ok, "Could not deserialize Protobuf message");
}
