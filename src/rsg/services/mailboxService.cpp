#include "rsg/services.hpp"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>

using namespace ::apache::thrift::server;
using namespace  ::RsgService;

using namespace ::simgrid;


int64_t rsg::RsgMailboxHandler::mb_create(const std::string& name) {
  s4u::Mailbox *mbox = s4u::Mailbox::byName(name.c_str());
  return (int64_t) mbox;
};

void rsg::RsgMailboxHandler::setReceiver(const int64_t remoteAddrMailbox, const int64_t remoteAddrActor) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) remoteAddrMailbox;
  s4u::Actor *actor = (s4u::Actor*) remoteAddrActor;
  mbox->setReceiver(*actor);
}

int64_t rsg::RsgMailboxHandler::getReceiver(const int64_t remoteAddrMailbox) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) remoteAddrMailbox;
  return (int64_t) (new s4u::Actor(mbox->receiver()));
}
