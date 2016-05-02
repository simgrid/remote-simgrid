#include "rsg/comm.hpp"
#include "client/RsgClientEngine.hpp"

#include <bitset>
#include <iostream>

boost::shared_ptr<RsgCommClient> rsg::Comm::pCommService(NULL);

rsg::Comm::Comm(unsigned long int remoteAddr) : p_remoteAddr(remoteAddr) {

}

rsg::Comm &rsg::Comm::send_init(rsg::Actor *sender, rsg::Mailbox &dest) {
  if(!pCommService) initNetworkService();
  rsg::Comm &res = *(new rsg::Comm(pCommService->send_init(0, dest.p_remoteAddr))); // FIXME memory leak
  return res;
}

rsg::Comm &rsg::Comm::send_async(rsg::Actor *sender, rsg::Mailbox &dest, void *data, int simulatedByteAmount) {
  if(!pCommService) initNetworkService();
  rsg::Comm &res = *(new rsg::Comm(pCommService->send_init(0, dest.p_remoteAddr))); // FIXME memory leak

  //res.setRemains(simulatedByteAmount);
  res.srcBuff_ = data;
  res.srcBuffSize_ = sizeof(void*);

  res.start();
  return res;
}

rsg::Comm &rsg::Comm::recv_init(rsg::Actor *receiver, rsg::Mailbox &from) {
  if(!pCommService) initNetworkService();
  return *(new rsg::Comm(pCommService->recv_init(0, from.p_remoteAddr)));
}

rsg::Comm &rsg::Comm::recv_async(rsg::Actor *receiver, rsg::Mailbox &from, void **data) {
  rsg::Comm &res = *(new rsg::Comm(pCommService->recv_async(0, from.p_remoteAddr))); //FIXME memory leaks
  res.setDstData(data);
  return res;
}

void rsg::Comm::initNetworkService() {
	ClientEngine& engine = ClientEngine::getInstance();
	pCommService = boost::shared_ptr<RsgCommClient>(engine.serviceClientFactory<RsgCommClient>("RsgComm"));
}

void rsg::Comm::start() {
  pCommService->start(p_remoteAddr);
}

void rsg::Comm::setSrcData(void *data) {
  std::string dataStr((char*)data);
  pCommService->setSrcData(p_remoteAddr, dataStr);
}

void rsg::Comm::setSrcDataSize(size_t size) {
  pCommService->setSrcDataSize(p_remoteAddr, size);
}

size_t rsg::Comm::getDstDataSize() {
  return pCommService->getDstDataSize(p_remoteAddr);
}

void rsg::Comm::wait() {
  if (dstBuff_ != NULL) {
    std::string res;
    pCommService->wait(res, p_remoteAddr);
    char * chars = (char*) malloc((res.size() * sizeof(char*) + 1));
    sprintf(chars, "%s", res.c_str());
    *(void**) this->dstBuff_ = (char *) chars;
    printf("END WAIT %s\n",chars);
  } else {
    std::string res;
    pCommService->wait(res, p_remoteAddr);
    printf("END WAIT \n");
  }
}

void rsg::Comm::setDstData(void **buff) {
  this->dstBuff_ = buff;
}

void rsg::Comm::setDstData(void ** buff, size_t size) {
  this->dstBuff_ = buff;
  pCommService->setDstData(p_remoteAddr, size);
}
