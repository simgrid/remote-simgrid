#pragma once

#include <iostream>
#include <mutex>
#include <map>
#include <unordered_map>

#include <simgrid/s4u.hpp>

#include <thrift/TProcessor.h>
#include <thrift/server/TServer.h>

#include <zmq.hpp>

#include "../common.hpp"

namespace apache { namespace thrift { namespace server {

/**
 * @brief Thrift ZeroMQ Server class
 * @details Network architecture:
 * 1. A client connect to the frontend server (ipc:///tmp/rsg.frontend.ipc)
 * 2. The client give a unique name to his session using an identity
 * 3. The frontend server receive data on the ROUTER socket
 * 4. it forwards the data to a backend on a inproc socket named after the name of the session (inproc://backend.NAME.inproc)
 * 5. all sockets use the REP/REQ model (clients wait for a reponse of the server before replying)
 *
 * TODO: add a second frontend for tcp connections.
 */
class TZmqServer : public TServer {
// subtypes
public:
    struct ConnectionStatus {
        // attributes
        bool waiting_for_initial_connection = true;
        std::map<std::string, std::string> expected_env;

        // methods
        std::string as_json() const;
    };

    class SharedConnectionsStatus {
    public:
        SharedConnectionsStatus();
        SharedConnectionsStatus(const SharedConnectionsStatus & oth) = delete;
        ~SharedConnectionsStatus();

        void insert_new_connection(const std::string & name);
        void mark_connection_as_initialized(const std::string & name);

        std::map<std::string, ConnectionStatus> all();
        std::map<std::string, ConnectionStatus> waiting_init();
    public:
        static std::string as_json(const std::map<std::string, ConnectionStatus> & statuses);

    private:
        std::map<std::string, ConnectionStatus> statuses;
        std::mutex mutex;
    };

// methods
public:
    TZmqServer(std::shared_ptr<TProcessor> processor, const std::string name, bool* server_exit);
    ~TZmqServer();

    bool serveOne(int recv_flags = 0);
    void serve();

    zmq::socket_t& getSocket();

// attributes
private:
    std::shared_ptr<TProcessor> processor_;
    int zmq_type_;
    zmq::socket_t sock_;
    bool* server_exit_;
    std::string name_;

// static methods
public:
    static void* router_thread(void*arg);
    static zmq::context_t& getContext();
    static void get_new_endpoint(std::string& new_name);
    static std::string status_string_all();
    static std::string status_string_waiting_init();

// static attributes
private:
    static SharedConnectionsStatus s_connection_status;
};

}}} // apache::thrift::server
