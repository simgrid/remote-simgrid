#include "client/multiThreadedSingletonFactory.hpp"
#include "RsgMsg.hpp"
#include <iostream>

MultiThreadedSingletonFactory *MultiThreadedSingletonFactory::pInstance = NULL;
std::mutex MultiThreadedSingletonFactory::mtx;

MultiThreadedSingletonFactory& MultiThreadedSingletonFactory::getInstance() {
  if(pInstance == NULL) pInstance = new MultiThreadedSingletonFactory();
  return *pInstance;
}

ClientEngine &MultiThreadedSingletonFactory::getEngine(std::thread::id id) {
  MultiThreadedSingletonFactory::mtx.lock();
  ClientEngine *res;
  try {
    res = pEngines->at(id);
  } catch (std::out_of_range& e) {
    res = new ClientEngine("localhost", 9090);
    res->init();
    pEngines->insert({std::this_thread::get_id(), res});
  }
  MultiThreadedSingletonFactory::mtx.unlock();
  return *res;
}

ClientEngine &MultiThreadedSingletonFactory::getEngineOrCreate(std::thread::id id, int rpcPort) {
  ClientEngine *res;
  MultiThreadedSingletonFactory::mtx.lock();
  try {
    res = pEngines->at(id);
  } catch (std::out_of_range& e) {
    res = new ClientEngine("localhost", 9090);
    res->connectToRpc(rpcPort);
    pEngines->insert({std::this_thread::get_id(), res});
  }
  MultiThreadedSingletonFactory::mtx.unlock();
  return *res;
}

void MultiThreadedSingletonFactory::clearEngine(std::thread::id id) {
  MultiThreadedSingletonFactory::mtx.lock();
  try {
    ClientEngine *engine;
    engine = pEngines->at(id);
    engine->close();
    engine->reset();
    delete engine;
    pEngines->erase(id);
  } catch (std::out_of_range& e) {
    std::cerr << "Engine already cleared or not existing " << std::endl;
  }
  MultiThreadedSingletonFactory::mtx.unlock();
}

