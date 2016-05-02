#include "rsg/RsgServiceImpl.h"

#include "xbt.h"
#include "simgrid/s4u.h"

#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG_SERVICE, "Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_THRIFT_SERVICE, RSG_SERVICE , "RSG server (Remote SimGrid)");

RsgActorHandler::RsgActorHandler()  : pSelf(s4u::Actor::self()), pServer(NULL) {
}

void RsgActorHandler::setServer(TServerFramework *server) {
  pServer = server;
}

void RsgActorHandler::close() {
  pServer->stop();
}

void  RsgActorHandler::sleep(const double duration) {
  pSelf.sleep(duration);
  XBT_INFO("slept %f secondes", duration);
}

void RsgActorHandler::execute(const double flops) {
  xbt_ex_t e;

  TRY {
    XBT_INFO(" before execute %f flops", flops);
    pSelf.execute(flops);
    XBT_INFO("after execute %f flops", flops);

  }
  CATCH(e) {
    XBT_INFO("Damn, the server is not on ");
  }
}

void RsgActorHandler::send(const int64_t mbAddr, const std::string& content, const int64_t simulatedSize) {
  s4u::Mailbox *mbox = (s4u::Mailbox*)mbAddr;
  char *contentChar = (char*) content.c_str();
  pSelf.send(*mbox, xbt_strdup(contentChar), simulatedSize);
}

void RsgActorHandler::recv(std::string& _return, const int64_t mbAddr) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) mbAddr;
  char *content = (char*)pSelf.recv(*mbox);
  XBT_INFO("recv(%s) ~> %s", mbox->getName(), content);
  _return.assign(content);
}

//FIXME the three following function assume that you can only have the "self" actor.
void RsgActorHandler::getName(std::string& _return, const int64_t addr) {
  const char * c_name = pSelf.getName();
  _return.assign(c_name);
}

//FIXME
void RsgActorHandler::getHost(rsgHostCurrentResType& _return, const int64_t addr) {
  s4u::Host *host = pSelf.getHost();
  _return.name = host->name();
  _return.addr = (unsigned long int) host;
}

//FIXME
int32_t RsgActorHandler::getPid(const int64_t addr) {
  return pSelf.getPid();
}

//FIXME
void RsgActorHandler::setAutoRestart(const int64_t addr, const bool autorestart) {
  pSelf.setAutoRestart(autorestart);
}

//FIXME
void RsgActorHandler::setKillTime(const int64_t addr, const double time) {
  pSelf.setKillTime(time);
}

//FIXME
double RsgActorHandler::getKillTime(const int64_t addr) {
  return pSelf.getKillTime();
}

void RsgActorHandler::killAll() {
  s4u::Actor::killAll();
}

int64_t RsgMailboxHandler::mb_create(const std::string& name) {
  s4u::Mailbox *mbox = s4u::Mailbox::byName(name.c_str());
  return (int64_t) mbox;
};


RsgHostHandler::RsgHostHandler() : pSelf(*s4u::Host::current()) {
}

int64_t RsgHostHandler::by_name(const std::string& name) {

  XBT_INFO("CMD_HOST_GETBYNAME");
  const char *c_name = name.c_str();
  s4u::Host *host = s4u::Host::by_name(c_name);

  if(host == nullptr) {
    XBT_INFO("No such Host (%s)", name);
    return 0;
  }

  XBT_INFO("host get by name (%s)",host->name().c_str());

  return (int64_t)host;
}

void RsgHostHandler::current(rsgHostCurrentResType& _return) {
  XBT_INFO("CMD_HOST_CURRENT");
  s4u::Host *host = s4u::Host::current();
  _return.name = host->name();
  _return.addr = (unsigned long int) host;
}

double RsgHostHandler::speed(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  return (double) host->speed();
}

void RsgHostHandler::turnOn(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  XBT_INFO("turn On host -> %s ", host->name().c_str());
  host->turnOn();
}

void RsgHostHandler::turnOff(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  XBT_INFO("turn Off host -> %s", host->name().c_str());
  host->turnOff();
}

bool RsgHostHandler::isOn(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
    return host->isOn();
}

double RsgHostHandler::currentPowerPeak(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
	return host->currentPowerPeak();
}

double RsgHostHandler::powerPeakAt(const int64_t addr, const int32_t pstate_index) {
	s4u::Host *host = (s4u::Host*) addr;
	return host->powerPeakAt((int) pstate_index);
}

int32_t RsgHostHandler::pstatesCount(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
  XBT_INFO("pstatesCount -> %s ", host->name().c_str());
  return host->pstatesCount();
}

void RsgHostHandler::setPstate(const int64_t addr, const int32_t pstate_index) {
	s4u::Host *host = (s4u::Host*) addr;
  XBT_INFO("setPstate -> %s (%d) ", host->name().c_str(), pstate_index);
  host->setPstate(pstate_index);
}

int32_t RsgHostHandler::pstate(const int64_t addr) {
	s4u::Host *host = (s4u::Host*) addr;
  XBT_INFO("pstate -> %s ", host->name().c_str());
  return host->pstate();
}


int32_t RsgHostHandler::core_count(const int64_t addr) {
  s4u::Host *host = (s4u::Host*) addr;
  XBT_INFO("core count -> %s ", host->name().c_str());
  return host->core_count();
}

boost::unordered_map<const int64_t ,unsigned long int> *RsgCommHandler::buffers = new boost::unordered_map<const int64_t ,unsigned long int>();

RsgCommHandler::RsgCommHandler() {
}

int64_t RsgCommHandler::send_init(const int64_t sender, const int64_t dest) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) dest;
  s4u::Actor *actorSender = &s4u::Actor::self();
  XBT_INFO("RsgCommHandler::send_init %s -> %s", actorSender->getName(), mbox->getName());
  XBT_INFO("mb adress %p", dest);
  return (int64_t) &s4u::Comm::send_init(actorSender, *mbox);
}

int64_t RsgCommHandler::recv_init(const int64_t receiver, const int64_t from_) {
  s4u::Mailbox *mbox = (s4u::Mailbox*) from_;
  s4u::Actor *actorReceiver = &s4u::Actor::self();
  XBT_INFO("mb adress %p", from_);

  XBT_INFO("RsgCommHandler::recv_init %s -> %s", actorReceiver->getName(), mbox->getName());
  return (int64_t) &s4u::Comm::recv_init(actorReceiver, *mbox);
}

int64_t RsgCommHandler::recv_async(const int64_t receiver, const int64_t from_) {
  // s4u::Mailbox *mbox = (s4u::Mailbox*) from_;
  // s4u::Actor *actorReceiver = &s4u::Actor::self();
  // XBT_INFO("RsgCommHandler::recv_async %s -> %s", actorReceiver->getName(), mbox->getName());
  // void *buffer = (void*) malloc(sizeof(void*));
  // unsigned long int comm_addr = (int64_t)&s4u::Comm::recv_async(actorReceiver, *mbox, &buffer);
  // buffers->insert({comm_addr, buffer});
  // return comm_addr;
  return 0;
}

int64_t RsgCommHandler::send_async(const int64_t sender, const int64_t dest, const std::string& data, const int32_t simulatedByteAmount) {
  return 0;
}

void RsgCommHandler::start(const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->start();
}

void RsgCommHandler::wait(std::string& _return, const int64_t addr) {
  XBT_INFO("wait data");
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->wait();
  try {
    void **buffer = (void**) buffers->at((unsigned long int)addr);
    XBT_INFO("wait comm addr %p, buffer value -> %p ",comm);
    if(buffer) {
      printf("comm addr = %p, buffer -> %f\n",comm , *(double*)buffer);
    _return.assign((char*)buffer, sizeof(double));
    free(buffer);
    buffers->erase(addr);
    } else {
      xbt_die("Empty dst buffer");
    }
  } catch (std::out_of_range& e) {
    XBT_INFO("sender side");
	}
}

void RsgCommHandler::setSrcDataSize(const int64_t addr, const int64_t size) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->setSrcDataSize((size_t)size);
}

int64_t RsgCommHandler::getDstDataSize(const int64_t addr) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  return comm->getDstDataSize();
}

void RsgCommHandler::setRate(const int64_t addr, const double rate) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  comm->setRate(rate);
}

void RsgCommHandler::setSrcData(const int64_t addr, const std::string& buff) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  XBT_INFO(" data size %d", buff.size());
  char* binary = (char*) malloc(buff.size()*sizeof(char));
  memcpy(&binary, buff.c_str(), buff.size()*sizeof(char));
  comm->setSrcData((void*)binary, sizeof(void*));
}

void RsgCommHandler::setDstData(const int64_t addr, const int64_t size) {
  s4u::Comm *comm = (s4u::Comm*) addr;
  unsigned long int bufferAddr;
  unsigned long int ptr = (unsigned long int) malloc(sizeof(void*));
  bufferAddr = ptr;
  XBT_INFO("set dst data comm addr %p, buffer value -> %p ",comm, bufferAddr );
  comm->setDstData((void**) bufferAddr, sizeof(void*));
  buffers->insert({addr, bufferAddr});
}
