#pragma once

#include <string>
#include <vector>

int add_actor(const std::string & server_hostname, int server_port,
    const std::string & actor_name, const std::string & vhost_name,
    bool autoconnect, const std::string & command_to_run,
    const std::vector<std::string> command_args);
void kill(const std::string & server_hostname, int server_port);
void start(const std::string & server_hostname, int server_port);
void status(const std::string & server_hostname, int server_port);
