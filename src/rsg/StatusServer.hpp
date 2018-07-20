#pragma once

#include <zmq.hpp>

class StatusServer {
public:
    StatusServer(uint16_t port);
    StatusServer(const StatusServer & oth) = delete;
    ~StatusServer();

    void run();

    static void* run_helper(void * server)
    {
        static_cast<StatusServer *>(server)->run();
        return NULL;
    }

private:
    zmq::socket_t * _sock = nullptr;
    zmq::context_t _context;
};
