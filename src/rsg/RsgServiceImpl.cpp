#include "rsg/RsgServiceImpl.h"

#include "xbt.h"
#include "simgrid/s4u.h"

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

void  RsgServiceHandler::sleep(const double duration) {
  pSelf.sleep(duration);
  XBT_INFO("slept %d secondes", duration);
}

void RsgServiceHandler::execute(const double flops) {
  pSelf.execute(flops);
  XBT_INFO("execute %d flops", flops);
}

void RsgServiceHandler::send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize) {
  s4u::Mailbox *mbox = (s4u::Mailbox*)mbAddr;
  char *contentChar = (char*) content.c_str();
  pSelf.send(*mbox, xbt_strdup(contentChar), simulatedSize);
}

void RsgServiceHandler::recv(std::string& _return, const int64_t mbAddr) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) mbAddr;
  char *content = (char*)pSelf.recv(*mbox);
  XBT_INFO("recv(%s) ~> %s", mbox->getName(), content);
  _return.assign(content);
}


int64_t RsgMailboxHandler::mb_create(const std::string& name) {
  s4u::Mailbox *mbox = s4u::Mailbox::byName(name.c_str());
  return (int64_t) mbox;
};
