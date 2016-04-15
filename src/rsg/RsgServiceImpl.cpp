#include "rsg/RsgServiceImpl.h"

#include "xbt.h"

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_SERVICE, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_SERVICE, RSG_SERVICE , "RSG server (Remote SimGrid)");

RsgServiceHandler::RsgServiceHandler()  : pSelf(s4u::Actor::self()), pServer(NULL) {
}

void RsgServiceHandler::setServer(TServerFramework *server) {
  pServer = server;
}

void RsgServiceHandler::close() {
  pServer->stop();
}

void  RsgServiceHandler::sleep(const int32_t duration) {
  XBT_INFO("Received SLEEP %d", duration);
  pSelf.sleep(10);
}
