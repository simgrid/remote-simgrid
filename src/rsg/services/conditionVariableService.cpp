#include "rsg/services.hpp"

#include "simgrid/s4u.h"
#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

rsg::RsgConditionVariableHandler::RsgConditionVariableHandler() {}

int64_t rsg::RsgConditionVariableHandler::conditionVariableInit() {
  return (int64_t) (new s4u::ConditionVariable()); 
}

void rsg::RsgConditionVariableHandler::conditionVariableDestroy(const int64_t remoteAddr) {
  s4u::ConditionVariable *cond = (s4u::ConditionVariable*) remoteAddr;
  delete cond;
}

void rsg::RsgConditionVariableHandler::wait(const int64_t remoteAddr, const int64_t mutexAddr) {
  s4u::ConditionVariable *cond = (s4u::ConditionVariable*) remoteAddr;
  s4u::Mutex *mutex = (s4u::Mutex*) mutexAddr;
  std::unique_lock<s4u::Mutex> lock(*mutex, std::defer_lock);
  cond->wait(lock);
}

void rsg::RsgConditionVariableHandler::wait_for(const int64_t remoteAddr, const int64_t mutexAddr, const double timeout) {
  s4u::ConditionVariable *cond = (s4u::ConditionVariable*) remoteAddr;
  s4u::Mutex *mutex = (s4u::Mutex*) mutexAddr;
  std::unique_lock<s4u::Mutex> lock(*mutex, std::adopt_lock);
  cond->wait_for(lock, timeout);
}

void rsg::RsgConditionVariableHandler::notify(const int64_t remoteAddr) {
  ((s4u::ConditionVariable*) remoteAddr)->notify_one();
}

void rsg::RsgConditionVariableHandler::notify_all(const int64_t remoteAddr) {
  ((s4u::ConditionVariable*) remoteAddr)->notify_all();
}
