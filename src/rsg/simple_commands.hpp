#pragma once

#include <string>
#include <vector>

int add_actor(const std::string & server_hostname, int server_port,
    const std::string & actor_name, const std::string & vhost_name,
    bool autoconnect, bool fork, const std::string & command_to_run,
    const std::vector<std::string> & command_args);
int kill(const std::string & server_hostname, int server_port,
    const std::string & reason);
int start(const std::string & server_hostname, int server_port);
int status(const std::string & server_hostname, int server_port,
    int retry_timeout_ms);
