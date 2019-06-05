#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "status.hpp"
#include "../rsg.capnp.h"

void status(const std::string & server_hostname, int server_port)
{
    printf("Status on (host=%s, port=%d) is not implemented yet.\n",
        server_hostname.c_str(), server_port);

    // Allocate a Command message, whose type is "status".
    ::capnp::MallocMessageBuilder message;
    Command::Builder command = message.initRoot<Command>();
    command.getType().setStatus();
}
