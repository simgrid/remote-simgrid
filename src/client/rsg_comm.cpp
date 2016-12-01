#include "rsg/comm.hpp"
#include "RsgClient.hpp"

#include "../rsg/services.hpp"


#include <bitset>
#include <iostream>

using namespace ::simgrid;

rsg::Comm::Comm(unsigned long int remoteAddr) : p_remoteAddr(remoteAddr) {
    
}

rsg::Comm::~Comm() {
    
}

rsg::Comm &rsg::Comm::send_init(rsg::Mailbox &dest) {
    rsg::Comm &res = *(new rsg::Comm(client.comm->send_init(0, dest.p_remoteAddr))); 
    return res;
}

rsg::Comm &rsg::Comm::send_async(rsg::Mailbox &dest, void *data, int simulatedByteAmount) {
    xbt_die("Size is needed in remote-simgrid. Please use send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size, int simulatedByteAmount) instead");
}

rsg::Comm &rsg::Comm::send_async(rsg::Mailbox &dest, void *data) {
    xbt_die("Size is needed in remote-simgrid. Please use send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size) instead");
}

//TODO Use the simulated amount
rsg::Comm &rsg::Comm::send_async(rsg::Mailbox &dest, void *data, size_t size, int simulatedByteAmount) {
    std::string dataStr((char*) data, size);
    
    rsg::Comm &res = *(new rsg::Comm(client.comm->send_async(0, dest.p_remoteAddr, dataStr, size, simulatedByteAmount)));
    return res;
}

rsg::Comm &rsg::Comm::send_async(rsg::Mailbox &dest, void *data, size_t size) {
    return rsg::Comm::send_async(dest, data, size, size);
}


rsg::Comm &rsg::Comm::recv_init(rsg::Mailbox &from) {
    return *(new rsg::Comm(client.comm->recv_init(0, from.p_remoteAddr)));
}

rsg::Comm &rsg::Comm::recv_async(rsg::Mailbox &from, void **data) {
    rsg::Comm &res = *(new rsg::Comm(client.comm->recv_async(0, from.p_remoteAddr))); 
    res.dstBuff_ = data;
    return res;
}

void rsg::Comm::start() {
    client.comm->start(p_remoteAddr);
}

void rsg::Comm::setSrcData(void *data, size_t size) {
    this->srcBuffSize_ = size;
    size_t dataSize = sizeof(char) * this->srcBuffSize_;
    char* buffer = (char*) malloc(dataSize);
    memcpy(buffer, data, dataSize);
    std::string dataStr((char*) buffer, size);
    client.comm->setSrcData(p_remoteAddr, dataStr);
    free(buffer);
}

void rsg::Comm::setSrcDataSize(size_t size) {
    xbt_die("unsupported : use setSrcData(void *data, size_t size) to set both the data and the size");
}

void rsg::Comm::setSrcData(void *data) {
    xbt_die("unsupported : use setSrcData(void *data, size_t size) to set both the data and the size");
}


size_t rsg::Comm::getDstDataSize() {
    return client.comm->getDstDataSize(p_remoteAddr);
}

void rsg::Comm::wait() {
    if (dstBuff_ != NULL) {
        std::string res;
        client.comm->waitComm(res, p_remoteAddr);
        char * chars = (char*) malloc(res.size());
        memcpy(chars, res.c_str(), res.size());
        *(void**) this->dstBuff_ = (char *) chars;
    } else {
        std::string res;
        client.comm->waitComm(res, p_remoteAddr);
    }
    delete this;
}

void rsg::Comm::setDstData(void **buff) {
    this->dstBuff_ = buff;
    client.comm->setDstData(p_remoteAddr);
}

void rsg::Comm::setDstData(void ** buff, size_t size) {
    this->dstBuff_ = buff;
    this->dstBuffSize_ = size;
    client.comm->setDstData(p_remoteAddr);
}

bool rsg::Comm::test() {
    rsgCommBoolAndData res;
    client.comm->test(res, p_remoteAddr);
    if(res.cond) {
        if (dstBuff_ != NULL) {
            char * chars = (char*) malloc(res.data.size());
            memcpy(chars, res.data.c_str(), res.data.size());
            *(void**) this->dstBuff_ = (char *) chars;
        }
        delete this;
        return true;
    } 
    return false;
}

void rsg::Comm::waitAnyWrapper(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms) {
    client.comm->wait_any(_return, comms);
}

void rsg::Comm::waitAnyForWrapper(rsgCommIndexAndData& _return, const std::vector<int64_t> & comms, const double timeout) {
    client.comm->wait_any_for(_return, comms, timeout);
}
