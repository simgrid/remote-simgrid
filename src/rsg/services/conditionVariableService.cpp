#include "rsg/services.hpp"

#include "simgrid/s4u.hpp"
#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;

std::unordered_map<int, simgrid::s4u::ConditionVariablePtr> rsg::RsgConditionVariableHandler::pConditionVariables;
unsigned long long rsg::RsgConditionVariableHandler::pConditionVariablesMapId = 0;

rsg::RsgConditionVariableHandler::RsgConditionVariableHandler() {}

int64_t rsg::RsgConditionVariableHandler::conditionVariableInit() {
  s4u::ConditionVariablePtr cond = s4u::ConditionVariable::createConditionVariable();
  unsigned long long newId = rsg::RsgConditionVariableHandler::pConditionVariablesMapId++;
  rsg::RsgConditionVariableHandler::pConditionVariables.insert({newId, cond});
  return newId;
}

void rsg::RsgConditionVariableHandler::conditionVariableDestroy(const int64_t remoteAddr) {
  rsg::RsgConditionVariableHandler::pConditionVariables.erase(remoteAddr);
}

void rsg::RsgConditionVariableHandler::wait(const int64_t remoteAddr, const int64_t mutexAddr) {
  s4u::ConditionVariablePtr cond = rsg::RsgConditionVariableHandler::pConditionVariables.at(remoteAddr);
  
  s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(mutexAddr);
  std::unique_lock<s4u::Mutex> lock(*mutex, std::defer_lock);
  cond->wait(lock);
}

rsgConditionVariableStatus::type rsg::RsgConditionVariableHandler::wait_for(const int64_t remoteAddr, const int64_t mutexAddr, const double timeout) {
  s4u::ConditionVariablePtr cond = rsg::RsgConditionVariableHandler::pConditionVariables.at(remoteAddr);
  s4u::MutexPtr mutex = rsg::RsgMutexHandler::pMutexes.at(mutexAddr);
  std::unique_lock<s4u::Mutex> lock(*mutex, std::defer_lock);
  std::cv_status status = cond->wait_for(lock, timeout);
  if(status == std::cv_status::timeout) {
    return rsgConditionVariableStatus::type::cv_timeout;
  } else {
    return rsgConditionVariableStatus::type::cv_no_timeout;
  }
}

void rsg::RsgConditionVariableHandler::notify(const int64_t remoteAddr) {
  s4u::ConditionVariablePtr cond = rsg::RsgConditionVariableHandler::pConditionVariables.at(remoteAddr);
  cond->notify_one();
}

void rsg::RsgConditionVariableHandler::notify_all(const int64_t remoteAddr) {
  s4u::ConditionVariablePtr cond = rsg::RsgConditionVariableHandler::pConditionVariables.at(remoteAddr);
  cond->notify_all();
}
