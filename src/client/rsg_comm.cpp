#include "rsg/comm.hpp"
#include "client/RsgClientEngine.hpp"
#include "client/multiThreadedSingletonFactory.hpp"


#include <bitset>
#include <iostream>

using namespace ::simgrid;

rsg::Comm::Comm(unsigned long int remoteAddr) : p_remoteAddr(remoteAddr) {

}

rsg::Comm::~Comm() {

}

rsg::Comm &rsg::Comm::send_init(rsg::Mailbox &dest) {
  
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  rsg::Comm &res = *(new rsg::Comm(commService.send_init(0, dest.p_remoteAddr))); // FIXME memory leak
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
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  std::string dataStr((char*) data, size);
  
  rsg::Comm &res = *(new rsg::Comm(commService.send_async(0, dest.p_remoteAddr, dataStr, size, simulatedByteAmount)));
  return res;
}

rsg::Comm &rsg::Comm::send_async(rsg::Mailbox &dest, void *data, size_t size) {
  
  return rsg::Comm::send_async(dest, data, size, size);
}


rsg::Comm &rsg::Comm::recv_init(rsg::Mailbox &from) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  return *(new rsg::Comm(commService.recv_init(0, from.p_remoteAddr)));
}

rsg::Comm &rsg::Comm::recv_async(rsg::Mailbox &from, void **data) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  rsg::Comm &res = *(new rsg::Comm(commService.recv_async(0, from.p_remoteAddr))); 
  res.dstBuff_ = data;
  return res;
}

void rsg::Comm::start() {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient& commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  commService.start(p_remoteAddr);
}

void rsg::Comm::setSrcData(void *data, size_t size) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  this->srcBuffSize_ = size;
  size_t dataSize = sizeof(char) * this->srcBuffSize_;
  char* buffer = (char*) malloc(dataSize);
  memcpy(buffer, data, dataSize);
  std::string dataStr((char*) buffer, size);
  commService.setSrcData(p_remoteAddr, dataStr);
  free(buffer);
}

void rsg::Comm::setSrcDataSize(size_t size) {
  xbt_die("unsupported : use setSrcData(void *data, size_t size) to set both the data and the size");
}

void rsg::Comm::setSrcData(void *data) {
  xbt_die("unsupported : use setSrcData(void *data, size_t size) to set both the data and the size");
}


size_t rsg::Comm::getDstDataSize() {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  return commService.getDstDataSize(p_remoteAddr);
}

void rsg::Comm::wait() {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  if (dstBuff_ != NULL) {
    std::string res;
    commService.waitComm(res, p_remoteAddr);
    char * chars = (char*) malloc(res.size());
    memcpy(chars, res.c_str(), res.size());
    *(void**) this->dstBuff_ = (char *) chars;
  } else {
    std::string res;
    commService.waitComm(res, p_remoteAddr);
  }
  delete this;
}

void rsg::Comm::setDstData(void **buff) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  this->dstBuff_ = buff;
  commService.setDstData(p_remoteAddr);
}

void rsg::Comm::setDstData(void ** buff, size_t size) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

  RsgCommClient commService = engine.serviceClientFactory<RsgCommClient>("RsgComm");
  
  this->dstBuff_ = buff;
  this->dstBuffSize_ = size;
  commService.setDstData(p_remoteAddr);
}
