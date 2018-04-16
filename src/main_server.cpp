#include <stdlib.h>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/program_options/parsers.hpp>

// #include <thrift/processor/TMultiplexedProcessor.h>

// #include "xbt.h"
#include "simgrid/s4u.hpp"

#include "rsg/TZmqServer.hpp"
#include "rsg/RsgThriftServer.hpp"
// #include "rsg/Socket.hpp"
// #include "rsg/services.hpp"
// #include "rsg/Server.hpp"
#include "common.hpp"


using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;

using boost::shared_ptr;
using namespace ::simgrid;

std::mutex print;
std::vector<std::thread*> threads;
bool start_clients;

XBT_LOG_NEW_CATEGORY(RSG_THRIFT, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_SERVER, RSG_THRIFT , "RSG server (Remote SimGrid)");

extern char **environ;

/*!
 *  Fork a remote process, creates an RPC server to handle all the actor communication
 *  The function first creates an RPC server with an unused TCP port. When the server is succefully setted in listen mode 
 *  the function forks and set the Rpc port to the environment with execve.  
*/
static int rsg_representative(int argc, char **argv) {
    
    std::string name;
    TZmqServer::get_new_endpoint(name);
    
    debug_server_print("STARTING %s", name.c_str());
    if (!start_clients)
    {
        printf("RsgRpcNetworkName=%s\n", name.c_str());
        fflush(stdout);
    }
    
    RsgThriftServer srv(name);

    //We create the string formated to environment variables (key=value)
    std::string envKeyValueStr = "RsgRpcNetworkName=" + name;

    if (start_clients)
    {
        // We fork the actor process
        pid_t pid = fork();
        if (! pid) {
            putenv((char*)envKeyValueStr.c_str());

            int newargc = argc-1+2+1;
            char **newargv = (char**)calloc(newargc, sizeof(char*));
            newargv[0] = (char*)"/usr/bin/env";
            newargv[1] = (char*)"--";
            for(int i=1; i < argc; i++) {
                newargv[1+i] = argv[i];
            }
            newargv[newargc-1] = NULL;

            debug_server_print("fork+exec: %s with ENV(%s)",
                               newargv[2], envKeyValueStr.c_str());
            execve(newargv[0], newargv, environ);
        }
        debug_server_print("Child started at pid %i", pid);
    }
    else
    {
        printf("NOT DOING fork+exec: %s with ENV(%s)\n",
               argv[1], envKeyValueStr.c_str());

        std::string command_str;
        for (int i = 1; i < argc-1; ++i)
        {
            command_str += (std::string)(argv[i]) + " ";
        }
        command_str += argv[argc-1];

        printf("Waiting for client external launch. "
               "Environment: %s. "
               "Command: %s\n",
               envKeyValueStr.c_str(),
               command_str.c_str());
    }

    srv();

    debug_server_print("END");
    return 0;
}

int main(int argc, char **argv) {
    std::string platform_file;
    std::string deployment_file;
    start_clients = true;

    namespace po = boost::program_options;
    po::options_description desc("Options description");
    desc.add_options()
            ("help,h", "print usage message")
            ("server-only",
             "prevents the server to fork+exec into clients to deploy")
            ("platform-file", po::value(&platform_file)->required(),
             "the SimGrid platform to simulate")
            ("deployment-file", po::value(&deployment_file)->required(),
             "the SimGrid initial deployment")
            ;

    po::positional_options_description positional_options;
    positional_options.add("platform-file", 1);
    positional_options.add("deployment-file", 1);

    po::variables_map vm;

    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc)
                  .positional(positional_options).run(),
                  vm); // throws on error

        if (vm.count("help") > 0)
        {
            printf("Usage : %s platform_file deployment_file [options]\n",
                   argv[0]);
            std::cout << desc << "\n";
            return 0;
        }

        if (vm.count("server-only"))
        {
            start_clients = false;
        }

        po::notify(vm);
    }
    catch(boost::program_options::required_option& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
    catch(boost::program_options::error& e)
    {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }


    if (DEBUG_SERVER) {
        int major, minor, patch;
        zmq_version (&major, &minor, &patch);
        fprintf(stderr, DEBUG_PREFIX_STR "0MQ version is %i.%i.%i\n",
                major, minor, patch);
    }
    
    s4u::Engine *e = new s4u::Engine(&argc,argv);
    
    pthread_t router;
    pthread_create(&router, NULL, TZmqServer::router_thread, 0);
    
    // Initialize the SimGrid world
    e->loadPlatform(platform_file.c_str());
    e->registerDefault(rsg_representative);
    e->loadDeployment(deployment_file.c_str());
    e->run();
    
    //wait for the router to close cleanly
    pthread_join(router, 0);
    return 0;
}
