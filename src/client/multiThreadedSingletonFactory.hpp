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

#include "RsgClient.hpp"

class MultiThreadedSingletonFactory {
public:
    static MultiThreadedSingletonFactory& getInstance();
    Client &getClient(std::thread::id id);
    Client &getClientOrCreate(std::thread::id id, int rpcPort);
    void clearClient(std::thread::id id);
    void registerNewThread(std::thread *thread);
    void removeCurrentThread();
    void registerClient(Client *client);
    void flushAll();
    void clearAll(bool keepConnectionsOpen);
    // Client &getClientOrCreate(std::thread::id id);
    ~MultiThreadedSingletonFactory();
protected:
    MultiThreadedSingletonFactory() : pClients(new std::map<std::thread::id, Client*>()),                                     
                                      pThreads(new std::map<std::thread::id, std::thread*>()), 
                                      pMainThreadID(new size_t) {};
    // delete copy and move constructors and assign operators
    MultiThreadedSingletonFactory(MultiThreadedSingletonFactory const&) = delete;             // Copy construct
    MultiThreadedSingletonFactory(MultiThreadedSingletonFactory&&) = delete;                  // Move construct
    MultiThreadedSingletonFactory& operator=(MultiThreadedSingletonFactory const&) = delete;  // Copy assign
    MultiThreadedSingletonFactory& operator=(MultiThreadedSingletonFactory &&) = delete;      // Move assign
private:
    static MultiThreadedSingletonFactory* pInstance;
    static std::mutex mtx;           // mutex for critical section
    static std::mutex threadMutex;           // mutex for critical section

    //NOTE Maybe we do not need pointers 
    std::map<std::thread::id, Client*> *pClients;     
    std::map<std::thread::id, std::thread*> *pThreads; 

    //FIXME I had huge problem whith std::thread::id. If for some reason you have 
    //`thread::id of a non-executing thread` when a thread id is printed (with std::cout). Don't hesitate to remplace by hashes.
    // as its the case for pMainThreadID.
    size_t* pMainThreadID;
};


#endif /* _MULTITHREADED_SINGLETON_FACTORY_ */
