#include "client/multiThreadedSingletonFactory.hpp"
#include "RsgMsg.hpp"
#include <iostream>
#include <stack>
#include <algorithm>    // std::copy

MultiThreadedSingletonFactory *MultiThreadedSingletonFactory::pInstance = NULL;
std::mutex MultiThreadedSingletonFactory::mtx;
std::mutex MultiThreadedSingletonFactory::threadMutex;

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
    pMainThreadID = std::this_thread::get_id();
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
  
  if(id == pMainThreadID) {
    waitAll();
  }
  
}

void MultiThreadedSingletonFactory::registerNewThread(std::thread *thread) {
  MultiThreadedSingletonFactory::threadMutex.lock();
  pThreads->push_back(thread);
  MultiThreadedSingletonFactory::threadMutex.unlock();
}

void MultiThreadedSingletonFactory::waitAll() {
  
  typedef std::vector<std::thread*>::iterator it_type;
  
  std::stack<std::thread*> joined;
  std::vector<std::thread*> copied;
  MultiThreadedSingletonFactory::threadMutex.lock();
  bool stop = pThreads->empty();
  MultiThreadedSingletonFactory::threadMutex.unlock();
  
  while(!stop) {
    MultiThreadedSingletonFactory::threadMutex.lock();
    std::copy ( pThreads->begin(), pThreads->end(),   std::back_inserter(copied));
    MultiThreadedSingletonFactory::threadMutex.unlock();
    
    for(it_type iterator = copied.begin(); iterator != copied.end(); iterator++) {
      if((*iterator)->joinable()) {
        (*iterator)->join();
        joined.push(*iterator);
      } else {
        debug_process("Unjoinable thread reaming in the vector");
      }
    }
    
    while(!joined.empty()) {
        std::thread *t = joined.top();
        joined.pop();
        MultiThreadedSingletonFactory::threadMutex.lock();
        it_type elem = std::find (pThreads->begin(), pThreads->end(), t);        
        pThreads->erase(elem);
        MultiThreadedSingletonFactory::threadMutex.unlock();
        delete t;
    }
    
    sleep(0.01);
    
    MultiThreadedSingletonFactory::threadMutex.lock();
    stop = pThreads->empty();
    MultiThreadedSingletonFactory::threadMutex.unlock();
    copied.clear();
  }  
}


