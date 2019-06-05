#include <map>
#include <string>
#include <iostream>

#include <docopt/docopt.h>

int main(int argc, char * argv[])
{
    static const char usage[] =
R"(Remote SimGrid command-line tool.

Usage:
  rsg serve [options]
  rsg add-actor <vhost> [options] [--] <command> [<command-args>...]
  rsg start [options]
  rsg status [options]
  rsg kill [options]
  rsg --help

Options:
  -h --hostname <host>  Server's TCP hostname [default: 127.0.0.1].
  -p --port <port>      Server's TCP port [default: 35000].
)";

    auto args = docopt::docopt(usage, { argv + 1, argv + argc }, true);

    std::cout << "Arguments:" << std::endl;
    for(auto const & arg : args) {
        std::cout << "  " << arg.first << ": " << arg.second << std::endl;
    }

    return 0;
}
