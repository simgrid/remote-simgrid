#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;


boost::unordered_map<const int64_t ,unsigned long int> *rsg::RsgCommHandler::buffers = new boost::unordered_map<const int64_t ,unsigned long int>();

rsg::RsgCommHandler::RsgCommHandler() {
}

int64_t rsg::RsgCommHandler::send_init(const int64_t sender, const int64_t dest) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) dest;
  s4u::Actor *actorSender = &s4u::Actor::self();
  return (int64_t) &s4u::Comm::send_init(actorSender, *mbox);
}

int64_t rsg::RsgCommHandler::recv_init(const int64_t receiver, const int64_t from_) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) from_;
  s4u::Actor *actorReceiver = &s4u::Actor::self();

  return (int64_t) &s4u::Comm::recv_init(actorReceiver, *mbox);
}

int64_t rsg::RsgCommHandler::recv_async(const int64_t receiver, const int64_t from_) {
  // s4u::Mailbox *mbox = (s4u::Mailbox*) from_;
  // s4u::Actor *actorReceiver = &s4u::Actor::self();
  // XBT_INFO("rsg::RsgCommHandler::recv_async %s -> %s", actorReceiver->getName(), mbox->getName());
  // void *buffer = (void*) malloc(sizeof(void*));
  // unsigned long int comm_addr = (int64_t)&s4u::Comm::recv_async(actorReceiver, *mbox, &buffer);
  // buffers->insert({comm_addr, buffer});
  // return comm_addr;
  return 0;
}

int64_t rsg::RsgCommHandler::send_async(const int64_t sender, const int64_t dest, const std::string& data, const int32_t simulatedByteAmount) {
  return 0;
}

void rsg::RsgCommHandler::start(const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->start();
}

void rsg::RsgCommHandler::wait(std::string& _return, const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->wait();
  try {
    void **buffer = (void**) buffers->at((unsigned long int)addr);
    if(buffer) {
      std::string *res = (std::string*) *buffer;
      _return.assign(res->data(), res->length());
      delete res;
      free(buffer);
      buffers->erase(addr);
    } else {
      xbt_die("Empty dst buffer");
    }
  } catch (std::out_of_range& e) {

	}
}

void rsg::RsgCommHandler::setSrcDataSize(const int64_t addr, const int64_t size) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->setSrcDataSize((size_t)size);
}

int64_t rsg::RsgCommHandler::getDstDataSize(const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  return comm->getDstDataSize();
}

void rsg::RsgCommHandler::setRate(const int64_t addr, const double rate) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->setRate(rate);
}

void rsg::RsgCommHandler::setSrcData(const int64_t addr, const std::string& buff) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  std::string *payload = new std::string(buff.data(), buff.length());
  comm->setSrcData((void*)payload, sizeof(void*));
}

void rsg::RsgCommHandler::setDstData(const int64_t addr, const int64_t size) { //FIXME USE THE SIZE
  s4u::Comm *comm = (s4u::Comm*) addr;
  unsigned long int bufferAddr;
  unsigned long int ptr = (unsigned long int) malloc(sizeof(void*));
  bufferAddr = ptr;
  comm->setDstData((void**) bufferAddr, size);
  buffers->insert({addr, bufferAddr});
}
