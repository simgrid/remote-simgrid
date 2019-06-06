#pragma once

#include <string>
#include <vector>

// Contains information for sockets that just connected.
// Once a socket has declared it wants to become an actor socket,
// is is no longer managed by this thread nor uses this data structure.
struct BarelyConnectedSocketInformation
{
    size_t bytes_read = 0; // Stores how many bytes have been read.
    uint32_t content_size = 0; // Size of the following content.
    uint8_t * content_buffer = nullptr; // Content buffer. Initialized once content_size is known.

    ~BarelyConnectedSocketInformation();
};

void serve(const std::string & platform_file, int server_port, const std::vector<std::string> & simgrid_options);
