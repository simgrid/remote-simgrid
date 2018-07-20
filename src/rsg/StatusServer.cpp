#include "StatusServer.hpp"

#include <simgrid/s4u.hpp>

#include "TZmqServer.hpp"

#include "../common.hpp"

StatusServer::StatusServer(uint16_t port) {
    _sock = new zmq::socket_t(_context, ZMQ_REP);

    std::string bind_addr = "tcp://*:" + std::to_string(port);
    debug_server_print("[STATUS] bind %s", bind_addr.c_str());
    _sock->bind(bind_addr);
}

StatusServer::~StatusServer() {
    delete _sock;
    _sock = nullptr;
}

void StatusServer::run() {
    while (true) {
        zmq::message_t request;
        _sock->recv(&request);

        std::string request_str((char*)request.data(), request.size());
        std::string reply_str;
        debug_server_print("[STATUS] received request '%s'", request_str.c_str());

        if (request_str == "status all") {
            reply_str = apache::thrift::server::TZmqServer::status_string_all();
        } else if (request_str == "status waiting_init") {
            reply_str = apache::thrift::server::TZmqServer::status_string_waiting_init();
        } else {
            reply_str = R"({"error": "Unknown request received."})";
        }

        _sock->send(reply_str.c_str(), reply_str.size());
    }
}
