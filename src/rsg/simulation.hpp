#pragma once

#include <string>

#include "interthread_messaging.hpp"

void start_simulation_in_another_thread(const std::string & platform_file,
    const std::vector<std::string> & simgrid_options,
    rsg::message_queue * to_command);
