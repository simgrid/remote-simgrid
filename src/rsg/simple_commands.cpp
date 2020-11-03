#include <chrono>
#include <thread>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "simple_commands.hpp"
#include "../common/assert.hpp"
#include "../common/message.hpp"
#include "../common/network/tcp_socket.hpp"

#include "rsg.pb.h"

int add_actor(const std::string & server_hostname, int server_port,
    const std::string & actor_name, const std::string & vhost_name,
    const std::string & think_time,
    bool autoconnect, bool fork, const std::string & command_to_run,
    const std::vector<std::string> & command_args)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::pb::Command command;
        auto add_actor = new(rsg::pb::Command_AddActor);
        add_actor->set_actorname(actor_name);
        add_actor->set_hostname(vhost_name);
        command.set_allocated_addactor(add_actor);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::pb::CommandAck command_ack;
        read_message(command_ack, socket);

        socket.close();

        if (!command_ack.success())
        {
            printf("add-actor refused from server\n");
            return 1;
        }

        pid_t pid = -1;
        if (fork)
        {
            // Create the requested process
            errno = 0;
            pid = ::fork();
            RSG_ENFORCE(pid != -1, "Cannot create a new process: %s", strerror(errno));
        }

        if (!fork || pid == 0) // Child process (or parent if fork is disabled)
        {
            // Prepare execvp "argv".
            const size_t nb_args = command_args.size() + 2;
            char * args[nb_args];
            args[0] = strdup(command_to_run.c_str());
            for (size_t i = 0; i < nb_args - 2; i++)
                args[i+1] = strdup(command_args[i].c_str());
            args[nb_args-1] = nullptr;

            // Prepare environment variables.
            errno = 0;
            int ret = setenv("RSG_SERVER_HOSTNAME", server_hostname.c_str(), 1);
            RSG_ENFORCE(ret == 0, "Cannot setenv RSG_SERVER_HOSTNAME to '%s': %s",
                server_hostname.c_str(), strerror(errno));

            char port_string[16];
            ret = snprintf(port_string, 16, "%d", server_port);
            RSG_ENFORCE(ret > 0 && ret < 16, "Cannot convert port=%d to a 16-length string", server_port);
            ret = setenv("RSG_SERVER_PORT", port_string, 1);
            RSG_ENFORCE(ret == 0, "Cannot setenv RSG_SERVER_PORT to '%s': %s",
                port_string, strerror(errno));

            try
            {
                // Check that think_time has a supported syntax.
                if (think_time != "AS_MEASURED_BY_CLIENT") {
                    // Should be a double value, meaning a constant time increase.
                    errno = 0;
                    char* endptr = nullptr;
                    double think_time_double = strtod(think_time.c_str(), &endptr);
                    bool conversion_success = endptr != think_time.c_str() && *endptr == '\0' && errno != ERANGE;
                    RSG_ENFORCE(conversion_success, "think-time ('%s') is not a valid double", think_time.c_str());
                    RSG_ENFORCE(think_time_double >= 0, "think-time (%g) should not be negative", think_time_double);
                }
                ret = setenv("RSG_THINK_TIME", think_time.c_str(), 1);
                RSG_ENFORCE(ret == 0, "Cannot setenv RSG_THINK_TIME to '%s': %s", think_time.c_str(), strerror(errno));

                if (autoconnect)
                {
                    ret = setenv("RSG_AUTOCONNECT", "1", 1);
                    RSG_ENFORCE(ret == 0, "Cannot setenv RSG_AUTOCONNECT to '1': %s", strerror(errno));
                }
                else
                {
                    ret = unsetenv("RSG_AUTOCONNECT");
                    RSG_ENFORCE(ret == 0, "Cannot unsetenv RSG_AUTOCONNECT: %s", strerror(errno));
                }

                char actor_id_string[16];
                ret = snprintf(actor_id_string, 16, "%d", command_ack.actor().id());
                RSG_ENFORCE(ret > 0 && ret < 16, "Cannot convert actor_id=%d to a 16-length string", command_ack.actor().id());
                ret = setenv("RSG_INITIAL_ACTOR_ID", actor_id_string, 1);
                RSG_ENFORCE(ret == 0, "Cannot setenv RSG_INITIAL_ACTOR_ID to '%s': %s",
                    actor_id_string, strerror(errno));

                // Mute into the target process.
                errno = 0;
                execvp(command_to_run.c_str(), args);

                // This code is only executed if muting into the target process failed.
                printf("Could not mute into the target process: %s\n", strerror(errno));
                printf("execvp arguments were:\n");
                printf("- file: %s\n", command_to_run.c_str());
                printf("- argv: ['%s'", args[0]);
                for (size_t i = 1; i < nb_args; i++)
                    printf(", '%s'", args[i]);
                printf("]\n");
                RSG_ENFORCE(0, "Could not execvp into '%s'", command_to_run.c_str());
            }
            catch (const rsg::Error & e)
            {
                // Tell the server we could not run the command by killing it.
                printf("Killing the server, as executing the process failed.\n");
                char kill_reason[128];
                snprintf(kill_reason, 128, "add-actor (actor_id=%d) failed: %s",
                    command_ack.actor().id(), e.what());
                kill(server_hostname, server_port, std::string(kill_reason));

                return 1;
            }
        }
        // Parent
        return 0;
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
        return 1;
    }
}

int kill(const std::string & server_hostname, int server_port, const std::string & reason)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::pb::Command command;
        command.set_kill(reason);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::pb::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
        {
            printf("kill refused from server\n");
            return 1;
        }
        return 0;
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
        return 1;
    }
}

int start(const std::string & server_hostname, int server_port)
{
    try
    {
        // Connect to the server.
        rsg::TcpSocket socket;
        socket.connect(server_hostname, server_port);

        // Generate message.
        rsg::pb::Command command;
        command.set_start(true);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::pb::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
        {
            printf("start refused from server\n");
            return 1;
        }
        return 0;
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
        return 1;
    }
}

int status(const std::string & server_hostname, int server_port,
    int retry_timeout_ms)
{
    try
    {
        // Connect to the server.
        auto t0 = std::chrono::steady_clock::now();
        std::chrono::milliseconds timeout(retry_timeout_ms);
        rsg::TcpSocket socket;
        for (bool connected = false; !connected; )
        {
            try {
                socket.connect(server_hostname, server_port);
                connected = true;
            } catch (const rsg::Error & e) {
                if (std::chrono::steady_clock::now() - t0 < timeout)
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                else
                    throw;
            }
        }

        // Generate message.
        rsg::pb::Command command;
        command.set_status(true);

        // Write message on socket.
        write_message(command, socket);

        // Read acknowledment.
        rsg::pb::CommandAck command_ack;
        read_message(command_ack, socket);

        if (!command_ack.success())
        {
            printf("status refused from server\n");
            return 1;
        }
        else
        {
            printf("{\"nb_connected_actors\":%u}\n", command_ack.status().nbconnectedactors());
        }
        return 0;
    }
    catch (const rsg::Error & err)
    {
        printf("%s\n", err.what());
        return 1;
    }
}
