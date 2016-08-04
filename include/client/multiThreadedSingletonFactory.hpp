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
    Client &getClient(std::thread::id id);
    Client &getClientOrCreate(std::thread::id id, int rpcPort);
    void clearClient(std::thread::id id);
    void registerNewThread(std::thread *thread);
    void waitAll();
    void registerClient(Client *client);
    void clearAll(bool keepConnectionsOpen);
    // Client &getClientOrCreate(std::thread::id id);
protected:
    MultiThreadedSingletonFactory() : pClients(new std::map<std::thread::id, Client*>()), pThreads(new std::vector<std::thread*>()), pMainThreadID(new size_t) {};
private:
    static MultiThreadedSingletonFactory* pInstance;
    static std::mutex mtx;           // mutex for critical section
    static std::mutex threadMutex;           // mutex for critical section
    //FIXME I had huge problem whith std::thread::id. If for some reason you have 
    //`thread::id of a non-executing thread` when a thread id is printed (with std::cout). Don't hesitate to remplace by hashes.
    // as its the case for pMainThreadID.
    std::map<std::thread::id, Client*> *pClients; 
    std::vector<std::thread*> *pThreads; 
    size_t* pMainThreadID;
};


#endif /* _MULTITHREADED_SINGLETON_FACTORY_ */
