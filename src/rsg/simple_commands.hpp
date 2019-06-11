#pragma once

#include <string>

void add_actor(const std::string & server_hostname, int server_port,
    const std::string & actor_name, const std::string & vhost_name,
    bool autoconnect);
void kill(const std::string & server_hostname, int server_port);
void start(const std::string & server_hostname, int server_port);
void status(const std::string & server_hostname, int server_port);
