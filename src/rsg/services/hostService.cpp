#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"
#include "RsgMsg.hpp"
#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace  ::simgrid;

XBT_LOG_NEW_CATEGORY(SERVER,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, SERVER, "RSG::Actor");

rsg::RsgHostHandler::RsgHostHandler() : pSelf(*s4u::Host::current()) {
}

int64_t rsg::RsgHostHandler::by_name(const std::string& name) {
  debug_server("Reeived getting host by name");
  const char *c_name = name.c_str();
  s4u::Host *host = s4u::Host::by_name(c_name);

  if(host == nullptr) {
    xbt_die("No such Host (%s)", name);
    return 0;
  }
  debug_server("end of getting host by name");
  return (int64_t)host;
}

void rsg::RsgHostHandler::current(rsgHostCurrentResType& _return) {
  s4u::Host *host = s4u::Host::current();
  _return.name = host->name();
  _return.addr = (unsigned long int) host;
}

double rsg::RsgHostHandler::speed(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  return (double) host->speed();
}

void rsg::RsgHostHandler::turnOn(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  host->turnOn();
}

void rsg::RsgHostHandler::turnOff(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  host->turnOff();
}

bool rsg::RsgHostHandler::isOn(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
    return host->isOn();
}

double rsg::RsgHostHandler::currentPowerPeak(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
	return host->currentPowerPeak();
}

double rsg::RsgHostHandler::powerPeakAt(const int64_t addr, const int32_t pstate_index) {
	s4u::Host *host = (s4u::Host*) addr;
	return host->powerPeakAt((int) pstate_index);
}

int32_t rsg::RsgHostHandler::pstatesCount(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
  return host->pstatesCount();
}

void rsg::RsgHostHandler::setPstate(const int64_t addr, const int32_t pstate_index) {
	s4u::Host *host = (s4u::Host*) addr;
  host->setPstate(pstate_index);
}

int32_t rsg::RsgHostHandler::pstate(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
  return host->pstate();
}


int32_t rsg::RsgHostHandler::core_count(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  return host->core_count();
}
