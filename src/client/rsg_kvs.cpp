#include "rsg/kvs.hpp"
#include "RsgClient.hpp"

using namespace ::simgrid::rsg;


void kvs::get(std::string& _return, const std::string& key) {
  client->kvs->get(_return, key);
}

void kvs::remove(const std::string& key) {
  client->kvs->remove(key);
}

void kvs::replace(const std::string& key, const std::string& data) {
  client->kvs->replace(key, data);
}

void kvs::insert(const std::string& key, const std::string& data) {
  client->kvs->insert(key, data);
}
