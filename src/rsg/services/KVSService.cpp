#include <iostream>
#include <fstream>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include "../services.hpp"

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace ::apache::thrift::server;
using namespace ::RsgService;

std::shared_ptr<rsg::RsgKVSHandler> rsg::RsgKVSHandler::instance(nullptr);

std::shared_ptr<rsg::RsgKVSHandler> rsg::RsgKVSHandler::getInstance() {
  if(rsg::RsgKVSHandler::instance) 
  return std::shared_ptr<rsg::RsgKVSHandler>(instance);
  else {
    rsg::RsgKVSHandler::instance = std::shared_ptr<rsg::RsgKVSHandler>(new rsg::RsgKVSHandler());
    return std::shared_ptr<rsg::RsgKVSHandler>(instance);
  }
}

rsg::RsgKVSHandler::RsgKVSHandler() {
  //NOTE the environement variable allows us to keep the code lowly coupled.
  // As for the singleton part, I am willing to discuss of this part.
  char *data_entry_file = std::getenv("RSG_KVS_DATA");

  if(data_entry_file) {
    std::ifstream infile(data_entry_file);
    if( !infile  ) {
      std::cerr << "Can't open file " << std::string(data_entry_file) << std::endl;
      std::exit( -1 );
    }

    std::string line;
    while (std::getline(infile, line))
    {
      if(line.find("#") != 0) {
        std::string key, value;
        int separator = line.find(" ");
        if(line.size() < 3) {  // The minimal valide possible line is 'k v'; 
          continue; //Maybe we should fail instead.
        }

        //Key and value extraction.
        key   = line.substr(0, separator);
        value = line.substr(separator + 1, line.size());
        std::pair<std::string, std::string> pair(key, value);
        this->store.insert(pair);
      }
    }
  }
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
