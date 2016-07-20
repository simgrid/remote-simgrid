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
  s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);
  
  return (int64_t) &s4u::Comm::send_init(*mbox);
}

int64_t rsg::RsgCommHandler::recv_init(const int64_t receiver, const int64_t from_) {
  s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
  
  s4u::Comm &res = s4u::Comm::recv_init(*mbox);
  
  return (int64_t) &res; 
}

int64_t rsg::RsgCommHandler::recv_async(const int64_t receiver, const int64_t from_) {
  s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(from_);
  
  unsigned long int bufferAddr;
  unsigned long int ptr = (unsigned long int) malloc(sizeof(void*));
  bufferAddr = ptr;
  s4u::Comm &comm = s4u::Comm::recv_init(*mbox);
  comm.setDstData((void**) bufferAddr, sizeof(std::string*));
  buffers->insert({(int64_t) &comm, (unsigned long int) bufferAddr});
  return (int64_t) &comm;
}

int64_t rsg::RsgCommHandler::send_async(const int64_t sender, const int64_t dest, const std::string& data, const int64_t size, const int64_t simulatedByteAmount) {
  s4u::MailboxPtr mbox = rsg::RsgMailboxHandler::pMailboxes.at(dest);
  std::string *strData = new std::string(data.data(), data.length());
  return (int64_t) &s4u::Comm::send_async(*mbox, (void*) strData, simulatedByteAmount);
}

void rsg::RsgCommHandler::start(const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->start();
}

void rsg::RsgCommHandler::waitComm(std::string& _return, const int64_t addr) {
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

void rsg::RsgCommHandler::setDstData(const int64_t addr) { //FIXME USE THE SIZE
  s4u::Comm *comm = (s4u::Comm*) addr;
  unsigned long int bufferAddr;
  unsigned long int ptr = (unsigned long int) malloc(sizeof(void*));
  bufferAddr = ptr;
  comm->setDstData((void**) bufferAddr, sizeof(std::string*));
  buffers->insert({addr, bufferAddr});
}

void rsg::RsgCommHandler::test(rsgCommBoolAndData& _return, const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  if(comm->test()) {
    _return.cond = true;
    if (buffers->find((unsigned long int)addr) != buffers->end()) {
      void **buffer = (void**) buffers->at((unsigned long int)addr);
      if(buffer) {
        std::string *res = (std::string*) *buffer;
        _return.data.assign(res->data(), res->length());
        delete res;
        free(buffer);
        buffers->erase(addr);
      }
    }
  } else {    
    _return.data.assign("", 0);
    _return.cond = false;
  }
}

