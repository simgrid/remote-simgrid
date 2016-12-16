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

std::unordered_map<std::string, std::string> rsg::RsgKVSHandler::store;

void rsg::RsgKVSHandler::get(std::string& _return, const std::string& key) {
  try {
    auto val = rsg::RsgKVSHandler::store.at(key);
    _return.assign(val.data());
  } catch (std::exception &e) {
    _return.assign("");
  }
}

void rsg::RsgKVSHandler::remove(const std::string& key) {
  // Your implementation goes here
  printf("remove\n");
}

void rsg::RsgKVSHandler::replace(const std::string& key, const std::string& data) {
  // Your implementation goes here
  printf("replace\n");
}

void rsg::RsgKVSHandler::insert(const std::string& key, const std::string& data) {
  //Please note that insert wont insert your value if the key already exists in the map, (use replace instead).
  std::pair<std::string, std::string> pair(key, data);
  rsg::RsgKVSHandler::store.insert(pair);
}
