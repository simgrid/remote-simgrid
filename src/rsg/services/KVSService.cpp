#include "xbt.h"
#include "simgrid/s4u.hpp"
#include "../services.hpp"

#include <iostream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::RsgService;
using namespace ::simgrid;

shared_ptr<rsg::RsgKVSHandler> rsg::RsgKVSHandler::instance(nullptr);

shared_ptr<rsg::RsgKVSHandler> rsg::RsgKVSHandler::getInstance() {
  if(rsg::RsgKVSHandler::instance) 
    return shared_ptr<rsg::RsgKVSHandler>(instance);
  else {
    rsg::RsgKVSHandler::instance = shared_ptr<rsg::RsgKVSHandler>(new rsg::RsgKVSHandler());
    return shared_ptr<rsg::RsgKVSHandler>(instance);
  }
}

rsg::RsgKVSHandler::RsgKVSHandler() {
}

rsg::RsgKVSHandler::~RsgKVSHandler() {
}

void rsg::RsgKVSHandler::get(std::string& _return, const std::string& key) {
  try {
    auto val = this->store.at(key);
    _return.assign(val.data());
  } catch (std::exception &e) {
    _return.assign("");
  }
}

void rsg::RsgKVSHandler::remove(const std::string& key) {
  this->store.erase(key);
}

void rsg::RsgKVSHandler::replace(const std::string& key, const std::string& data) {
  //Please note that insert wont insert your value if the key already exists in the map, (use replace instead).
  auto it = RsgKVSHandler::store.find(key);
  if(it != RsgKVSHandler::store.end()) {
      it->second = std::string(data);
  } else {
    std::pair<std::string, std::string> pair(key, data);
    this->store.insert(pair);
  }
}

void rsg::RsgKVSHandler::insert(const std::string& key, const std::string& data) {
  //Please note that insert wont insert your value if the key already exists in the map, (use replace instead).
  std::pair<std::string, std::string> pair(key, data);
  this->store.insert(pair);
}

void rsg::RsgKVSHandler::take_lock() {
  this->sync.lock();
}

void rsg::RsgKVSHandler::release_lock() {
  this->sync.unlock();
}
