#include "../services.hpp"

#include "xbt.h"
#include "simgrid/s4u.hpp"

#include <iostream>
#include "../../common.hpp"
using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

std::unordered_map<int, simgrid::s4u::MutexPtr> rsg::RsgMutexHandler::pMutexes;
unsigned long long rsg::RsgMutexHandler::pMutexsMapId = 0;

int64_t rsg::RsgMutexHandler::mutexInit() {
    s4u::MutexPtr mutex = s4u::Mutex::createMutex();
    unsigned long long newId = rsg::RsgMutexHandler::pMutexsMapId++;
    rsg::RsgMutexHandler::pMutexes.insert({newId, mutex});
    return newId;
}

void rsg::RsgMutexHandler::lock(const int64_t rmtAddr) {
    FILE *f = fopen("debug_mtx", "a");
    try {
        s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(rmtAddr);
        fprintf(f, "(%d)[%p]::before lock\n",s4u::this_actor::getPid(), mutex);
        mutex->lock();
        fprintf(f, "(%d)[%p]::locked\n",s4u::this_actor::getPid(), mutex);
    } catch(const std::exception &e) {
        std::cerr <<  "No such mutex " << e.what() << std::endl;
    }
    fclose(f);
}

void rsg::RsgMutexHandler::unlock(const int64_t rmtAddr) {
    s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(rmtAddr);
    FILE *f = fopen("debug_mtx", "a");

    fprintf(f, "(%d)[%p]::before unlock\n",s4u::this_actor::getPid(), mutex);
    if(!mutex->try_lock()) {
        mutex->unlock();
    }

    if(mutex->try_lock()) {
        mutex->unlock();
    }
    fprintf(f, "(%d)[%p]::unlock\n",s4u::this_actor::getPid(), mutex);
    fclose(f);
}

bool rsg::RsgMutexHandler::try_lock(const int64_t rmtAddr) {
    s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(rmtAddr);
    return mutex->try_lock();
}

void rsg::RsgMutexHandler::destroy(const int64_t rmtAddr) {
    rsg::RsgMutexHandler::pMutexes.erase(rmtAddr);
}
