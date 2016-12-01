#include <stdlib.h>
#include <iostream>
#include <vector>

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
    
    // We fork the actor process
    pid_t pid = fork();
    if (! pid) {
        //We create the string formated to environment variables (key=value)
        std::string envKeyValueStr = "RsgRpcNetworkName=" + name;
        
        putenv((char*)envKeyValueStr.c_str());

        int newargc = argc-1+2+1;
        char **newargv = (char**)calloc(newargc, sizeof(char*));
        newargv[0] = (char*)"/usr/bin/env";
        newargv[1] = (char*)"--";
        for(int i=1; i < argc; i++) {
            newargv[1+i] = argv[i];
        }
        newargv[newargc-1] = NULL;
        debug_server_print("fork+exec: %s", newargv[2]);
        execve(newargv[0], newargv, environ);
    }
    debug_server_print("Child started at pid %i", pid);

    RsgThriftServer srv(name);
    srv();
    
    debug_server_print("END");
    return 0;
}

int main(int argc, char **argv) {
    if (DEBUG_SERVER) {
        int major, minor, patch;
        zmq_version (&major, &minor, &patch);
        printf("0MQ version is %i.%i.%i\n", major, minor, patch);
    }
    
    s4u::Engine *e = new s4u::Engine(&argc,argv);
    
    if (argc < 3) {
        fprintf(stderr, "Usage: rsg platform.xml deploy.xml port\n");
        exit(1);
    }
    
    pthread_t router;
    pthread_create(&router, NULL, TZmqServer::router_thread, 0);
    
    // Initialize the SimGrid world
    e->loadPlatform(argv[1]);
    e->registerDefault(rsg_representative);
    e->loadDeployment(argv[2]);
    e->run();
    
    //wait for the router to clsoe cleanly
    pthread_join(router, 0);
    return 0;
}
