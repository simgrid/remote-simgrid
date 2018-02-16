#pragma once

#include <zmq.hpp>
#include <thrift/transport/TBufferTransports.h>
#include <iostream>

#include "../common.hpp"

namespace apache { namespace thrift { namespace transport {

class TZmqClient : public TTransport {
public:
    TZmqClient(zmq::context_t& ctx, std::string name);
    ~TZmqClient();

    void open();
    uint32_t read_virt(uint8_t* buf, uint32_t len);
    void write_virt(const uint8_t* buf, uint32_t len);
    uint32_t writeEnd();

protected:
    zmq::socket_t sock_;
    std::string endpoint_;
    TMemoryBuffer wbuf_;
    TMemoryBuffer rbuf_;
    zmq::message_t msg_;
    int zmq_type_;
    std::string name_;
};

}}} // apache::thrift::transport
