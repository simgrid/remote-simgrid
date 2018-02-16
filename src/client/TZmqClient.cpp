#include <thrift/protocol/TProtocol.h>
#include <thrift/protocol/TVirtualProtocol.h>
#include <cstring>

#include "TZmqClient.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;

namespace apache { namespace thrift { namespace transport {

uint32_t TZmqClient::read_virt(uint8_t* buf, uint32_t len) {
    if (rbuf_.available_read() == 0) {
        debug_client_stream<<"[TZmqClient "<<name_<<"] recving "<<len<<debug_client_stream_end;
        bool ret = sock_.recv(&msg_);assert(ret==true);
        debug_client_stream<<"[TZmqClient "<<name_<<"] recvd something "<<msg_.size()<<debug_client_stream_end;
        int32_t asyncmsg = (int32_t)TNetworkBigEndian::fromWire32(*(uint32_t*)msg_.data());
        debug_client_print("[TZmqClient %s]EXTRA MESSAGE: %u ", name_.c_str(), asyncmsg);
        if(asyncmsg == SIGKILL) {
            //we do not do "exit()" as a simgrid process is in reality a thread.
            pthread_exit(0);
        }
        rbuf_.resetBuffer((uint8_t*)msg_.data()+4, msg_.size()-4);
    }
    return rbuf_.read(buf, len);
}

void TZmqClient::write_virt(const uint8_t* buf, uint32_t len) {
    return wbuf_.write(buf, len);
}

uint32_t TZmqClient::writeEnd() {
    uint8_t* buf;
    uint32_t size;
    wbuf_.getBuffer(&buf, &size);
    zmq::message_t msg(size);
    std::memcpy(msg.data(), buf, size);
    debug_client_stream<<"[TZmqClient "<<name_<<"] sending"<<debug_client_stream_end;
    bool ret = sock_.send(msg);assert(ret==true);
    wbuf_.resetBuffer(true);
    return size;
}

TZmqClient::TZmqClient(zmq::context_t &ctx, std::string name) :
    sock_(ctx, ZMQ_REQ),
    wbuf_(),
    rbuf_(),
    msg_(),
    zmq_type_(ZMQ_REQ),
    name_(name)
{
    endpoint_ = "ipc:///tmp/rsg.frontend.ipc";
    debug_client_stream<<"[TZmqClient "<<name_<<"] create @"<<endpoint_<<"   "<<this<<debug_client_stream_end;
    sock_.setsockopt(ZMQ_IDENTITY, name_.c_str(), name_.length());
}

TZmqClient::~TZmqClient()
{
    debug_client_stream<<"[TZmqClient "<<name_<<"] BYBYE"<<debug_client_stream_end;
}

void TZmqClient::open() {
    sock_.connect(endpoint_.c_str());
}

}}} // apache::thrift::transport
