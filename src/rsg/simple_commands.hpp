#pragma once

#include <string>

void kill(const std::string & server_hostname, int server_port);
void start(const std::string & server_hostname, int server_port);
void status(const std::string & server_hostname, int server_port);
