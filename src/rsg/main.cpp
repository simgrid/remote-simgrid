#include <map>
#include <string>
#include <iostream>

#include <docopt/docopt.h>

#include "serve.hpp"
#include "simple_commands.hpp"

int main(int argc, char * argv[])
{
    static const char usage[] =
R"(Remote SimGrid command-line tool.

Usage:
  rsg serve <platform-file> [--port=<port>] [-- <sg-options>...]
  rsg add-actor <actor-name> <sg-host> [--hostname=<host>] [--port=<port>]
                [--] <command> [<command-args>...]
  rsg start [--hostname=<host>] [--port=<port>]
  rsg status [--hostname=<host>] [--port=<port>]
  rsg kill [--hostname=<host>] [--port=<port>]
  rsg --help

Options:
  -h --hostname <host>  Server's hostname [default: 127.0.0.1].
  -p --port <port>      Server's TCP port [default: 35000].
)";

    // Parse CLI arguments.
    auto args = docopt::docopt(usage, { argv + 1, argv + argc }, true);

    // Check argument validity
    std::string server_hostname = args["--hostname"].asString();
    int server_port = args["--port"].asLong();

    // Debug printing, should be removed.
    /*std::cout << "Arguments:" << std::endl;
    for(auto const & arg : args) {
        std::cout << "  " << arg.first << ": " << arg.second << std::endl;
    }*/

    // Manage subcommands.
    if (args["serve"].asBool())
    {
        std::string platform_file = args["<platform-file>"].asString();
        std::vector<std::string> simgrid_options = args["<sg-options>"].asStringList();
        serve(platform_file, server_port, simgrid_options);
    }
    else if (args["kill"].asBool())
    {
        kill(server_hostname, server_port);
    }
    else if (args["start"].asBool())
    {
        start(server_hostname, server_port);
    }
    else if (args["status"].asBool())
    {
        status(server_hostname, server_port);
    }
    else if (args["add-actor"].asBool())
    {
        std::string actor_name = args["<actor-name>"].asString();
        std::string vhost_name = args["<sg-host>"].asString();
        add_actor(server_hostname, server_port, actor_name, vhost_name);
    }

    return 0;
}
