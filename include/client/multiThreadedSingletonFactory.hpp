/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef _MULTITHREADED_SINGLETON_FACTORY_
#define _MULTITHREADED_SINGLETON_FACTORY_

#include <xbt.h>
#include <vector>
#include <string>
#include <thread>
#include <map>
#include <mutex>          // std::mutex

#include "rsg/services.hpp"
#include "client/RsgClientEngine.hpp"

class MultiThreadedSingletonFactory {
  public:
	  static MultiThreadedSingletonFactory& getInstance();
    ClientEngine &getEngine(std::thread::id id);
    ClientEngine &getEngineOrCreate(std::thread::id id, int rpcPort);
    void clearEngine(std::thread::id id);

    // ClientEngine &getEngineOrCreate(std::thread::id id);
  protected:
    MultiThreadedSingletonFactory() : pEngines(new std::map<std::thread::id, ClientEngine*>()) {};
  private:
    static MultiThreadedSingletonFactory* pInstance;
    std::map<std::thread::id, ClientEngine*> *pEngines; 
    static std::mutex mtx;           // mutex for critical section

};


#endif /* _MULTITHREADED_SINGLETON_FACTORY_ */
