#include <map>
#include <string>
#include <iostream>

#include <docopt/docopt.h>

#include "serve.hpp"

int main(int argc, char * argv[])
{
    static const char usage[] =
R"(Remote SimGrid command-line tool.

Usage:
  rsg serve <platform-file> [--port=<port>]
  rsg add-actor <vhost> [options] [--] <command> [<command-args>...]
  rsg start [options]
  rsg status [options]
  rsg kill [options]
  rsg --help

Options:
  -h --hostname <host>  Server's hostname [default: 127.0.0.1].
  -p --port <port>      Server's TCP port [default: 35000].
)";

    // Parse CLI arguments.
    auto args = docopt::docopt(usage, { argv + 1, argv + argc }, true);

    // Check argument validity
    std::string platform_file = args["<platform-file>"].asString();
    // TODO: check that file exists?
    int port = args["--port"].asLong();
    // TODO: check port bounds

    // Debug printing, should be removed.
    std::cout << "Arguments:" << std::endl;
    for(auto const & arg : args) {
        std::cout << "  " << arg.first << ": " << arg.second << std::endl;
    }

    // Manage subcommands.
    if (args["serve"])
        serve(platform_file, port);

    return 0;
}
