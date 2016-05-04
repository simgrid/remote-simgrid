#include "rsg/comm.hpp"
#include "client/RsgClientEngine.hpp"

#include <bitset>
#include <iostream>

using namespace ::simgrid;

boost::shared_ptr<RsgCommClient> rsg::Comm::pCommService(NULL);

rsg::Comm::Comm(unsigned long int remoteAddr) : p_remoteAddr(remoteAddr) {

}

rsg::Comm::~Comm() {

}

rsg::Comm &rsg::Comm::send_init(rsg::Actor *sender, rsg::Mailbox &dest) {
  if(!pCommService) initNetworkService();
  rsg::Comm &res = *(new rsg::Comm(pCommService->send_init(0, dest.p_remoteAddr))); // FIXME memory leak
  return res;
}

rsg::Comm &rsg::Comm::send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, int simulatedByteAmount) {
  xbt_die("Size is needed in remote-simgrid. Please use send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size, int simulatedByteAmount) instead");
}

rsg::Comm &rsg::Comm::send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data) {
  xbt_die("Size is needed in remote-simgrid. Please use send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size) instead");
}

//TODO Use the simulated amount
rsg::Comm &rsg::Comm::send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size, int simulatedByteAmount) {
  if(!pCommService) initNetworkService();
  std::string dataStr((char*) data, size);
  
  rsg::Comm &res = *(new rsg::Comm(pCommService->send_async(0, dest.p_remoteAddr, dataStr, size, simulatedByteAmount)));
  return res;
}

rsg::Comm &rsg::Comm::send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, size_t size) {
  if(!pCommService) initNetworkService();
  return rsg::Comm::send_async(sender, dest, data, size, size);
}


rsg::Comm &rsg::Comm::recv_init(rsg::Actor *receiver, rsg::Mailbox &from) {
  if(!pCommService) initNetworkService();
  return *(new rsg::Comm(pCommService->recv_init(0, from.p_remoteAddr)));
}

rsg::Comm &rsg::Comm::recv_async(rsg::Actor *receiver, rsg::Mailbox &from, void **data) {
  rsg::Comm &res = *(new rsg::Comm(pCommService->recv_async(0, from.p_remoteAddr))); 
  res.dstBuff_ = data;
  return res;
}

void rsg::Comm::initNetworkService() {
	ClientEngine& engine = ClientEngine::getInstance();
	pCommService = boost::shared_ptr<RsgCommClient>(engine.serviceClientFactory<RsgCommClient>("RsgComm"));
}

void rsg::Comm::start() {
  pCommService->start(p_remoteAddr);
}

void rsg::Comm::setSrcData(void *data, size_t size) {
  this->srcBuffSize_ = size;
  size_t dataSize = sizeof(char) * this->srcBuffSize_;
  char* buffer = (char*) malloc(dataSize);
  memcpy(buffer, data, dataSize);
  std::string dataStr((char*) buffer, size);
  pCommService->setSrcData(p_remoteAddr, dataStr);
  free(buffer);
}

void rsg::Comm::setSrcDataSize(size_t size) {
  xbt_die("unsupported : use setSrcData(void *data, size_t size) to set both the data and the size");
}

void rsg::Comm::setSrcData(void *data) {
  xbt_die("unsupported : use setSrcData(void *data, size_t size) to set both the data and the size");
}


size_t rsg::Comm::getDstDataSize() {
  return pCommService->getDstDataSize(p_remoteAddr);
}

void rsg::Comm::wait() {
  if (dstBuff_ != NULL) {
    std::string res;
    pCommService->wait(res, p_remoteAddr);
    char * chars = (char*) malloc(res.size());
    memcpy(chars, res.c_str(), res.size());
    *(void**) this->dstBuff_ = (char *) chars;
  } else {
    std::string res;
    pCommService->wait(res, p_remoteAddr);
  }
  delete this;
}

void rsg::Comm::setDstData(void **buff) {
  this->dstBuff_ = buff;
  pCommService->setDstData(p_remoteAddr);
}

void rsg::Comm::setDstData(void ** buff, size_t size) {
  this->dstBuff_ = buff;
  this->dstBuffSize_ = size;
  pCommService->setDstData(p_remoteAddr);
}
