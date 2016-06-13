/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNUClient Affero Licence (see in file LICENCE).        */

#ifndef _MULTITHREADED_SINGLETON_FACTORY_
#define _MULTITHREADED_SINGLETON_FACTORY_

#include <xbt.h>
#include <vector>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <mutex>          // std::mutex

#include "rsg/services.hpp"
#include "client/RsgClient.hpp"

class MultiThreadedSingletonFactory {
  public:
	  static MultiThreadedSingletonFactory& getInstance();
    Client &getEngine(std::thread::id id);
    Client &getEngineOrCreate(std::thread::id id, int rpcPort);
    void clearEngine(std::thread::id id);
    void registerNewThread(std::thread *thread);
    void waitAll();
    // Client &getEngineOrCreate(std::thread::id id);
  protected:
    MultiThreadedSingletonFactory() : pEngines(new std::map<std::thread::id, Client*>()), pThreads(new std::vector<std::thread*>()) {};
  private:
    static MultiThreadedSingletonFactory* pInstance;
    static std::mutex mtx;           // mutex for critical section
    static std::mutex threadMutex;           // mutex for critical section
    std::map<std::thread::id, Client*> *pEngines; 
    std::vector<std::thread*> *pThreads; 
    std::thread::id pMainThreadID;

};


#endif /* _MULTITHREADED_SINGLETON_FACTORY_ */
