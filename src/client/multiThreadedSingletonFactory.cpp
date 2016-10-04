#include "multiThreadedSingletonFactory.hpp"
#include "../common.hpp"
#include "../rsg/services.hpp"
#include <iostream>
#include <stack>
#include <algorithm>    // std::copy

MultiThreadedSingletonFactory *MultiThreadedSingletonFactory::pInstance = NULL;
std::mutex MultiThreadedSingletonFactory::mtx;
std::mutex MultiThreadedSingletonFactory::threadMutex;

MultiThreadedSingletonFactory& MultiThreadedSingletonFactory::getInstance()  {
    if(pInstance == NULL) {
        pInstance = new MultiThreadedSingletonFactory();
    }
    return *pInstance;
}

MultiThreadedSingletonFactory::~MultiThreadedSingletonFactory() {
    delete pClients;
    delete pThreads;
    delete pMainThreadID;
}

Client &MultiThreadedSingletonFactory::getClient(std::thread::id id) {
    MultiThreadedSingletonFactory::mtx.lock();
    Client *res;
    std::hash<std::thread::id> hasher;
    try {
        res = pClients->at(id);
    } catch (std::out_of_range& e) {
        res = new Client("localhost");
        res->init();
        pClients->insert({std::this_thread::get_id(), res});
        *pMainThreadID = hasher(std::this_thread::get_id());        
    }
    MultiThreadedSingletonFactory::mtx.unlock();
    return *res;
}

void MultiThreadedSingletonFactory::registerClient(Client *client) {
    MultiThreadedSingletonFactory::mtx.lock();
    std::hash<std::thread::id> hasher;
    pClients->insert({std::this_thread::get_id(), client});
    if(pClients->size() == 1) {
        *pMainThreadID = hasher(std::this_thread::get_id());
    }    
    MultiThreadedSingletonFactory::mtx.unlock();
}

Client &MultiThreadedSingletonFactory::getClientOrCreate(std::thread::id id, int rpcPort) {
    Client *res;
    MultiThreadedSingletonFactory::mtx.lock();
    std::hash<std::thread::id> hasher;
    try {
        res = pClients->at(id);
    } catch (std::out_of_range& e) {
        res = new Client("localhost");
        res->connectToRpc(rpcPort);
        pClients->insert({std::this_thread::get_id(), res});
        if(pClients->size() == 1) {
            *pMainThreadID = hasher(std::this_thread::get_id());
        }
    }
    MultiThreadedSingletonFactory::mtx.unlock();
    return *res;
}

void MultiThreadedSingletonFactory::clearClient(std::thread::id id) {
    MultiThreadedSingletonFactory::mtx.lock();
    try {
        Client *client;
        client = pClients->at(id);
        client->close();
        client->reset();
        delete client;
        pClients->erase(id);
    } catch (std::out_of_range& e) {
        std::cerr << "Engine already cleared or not existing " << std::endl;
    }

    MultiThreadedSingletonFactory::mtx.unlock();
    std::hash<std::thread::id> hasher;
    size_t hash = hasher(id);
    removeCurrentThread();
    if(hash == *pMainThreadID) {
        //FIXME We need to discuss of how to manage the actors life span.
        // When I first design the waitAll, I thought we might want to spawn a main actor which spawn other actors.
        // and while the child are still alive, the main leave. But It leads to many sync problems with all the thread.
        // This design will force any actor to wait for their childs.
        MultiThreadedSingletonFactory::threadMutex.lock();
        if(pThreads->size() > 0) {
            std::cerr << "ERROR : rsg process is leaving whereas some actors may still be alives." << std::endl;
        }
        MultiThreadedSingletonFactory::threadMutex.unlock();
        
        delete pInstance;
    }
}

void MultiThreadedSingletonFactory::registerNewThread(std::thread *thread) {
    MultiThreadedSingletonFactory::threadMutex.lock();
    pThreads->insert({std::this_thread::get_id(), thread});
    MultiThreadedSingletonFactory::threadMutex.unlock();
}

void MultiThreadedSingletonFactory::removeCurrentThread() {
    MultiThreadedSingletonFactory::threadMutex.lock();
    pThreads->erase(std::this_thread::get_id());
    MultiThreadedSingletonFactory::threadMutex.unlock();
}

void MultiThreadedSingletonFactory::flushAll() {
    for(auto it = pClients->begin(); it != pClients->end(); ++it) {
        it->second->flush();
    }
}

void MultiThreadedSingletonFactory::clearAll(bool keepConnectionsOpen) {

    //FIXME The loop below should freeing all threads objects holding by the class however 
    // since the function is called from the child after a fork, the desctructor of the thread object cannot 
    // be called. One way to fix this would be to reimplements this class using pthread instead of std::threads.

    // for(auto it = pThreads->begin(); it != pThreads->end(); ++it) {
    //     if(((*it)->joinable()))
    //      (*it)->detach();
    //     delete *it;
    // }
    pThreads->clear();
    
    for(auto it = pClients->begin(); it != pClients->end(); ++it) {
        if(!keepConnectionsOpen) {
            it->second->close();
            it->second->reset();
        }
        delete it->second;
    }
    pClients->clear();
}


