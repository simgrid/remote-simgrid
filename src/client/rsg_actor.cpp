/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/actor.hpp"
#include "RsgMsg.hpp"
#include "client/RsgClientEngine.hpp"
#include "client/multiThreadedSingletonFactory.hpp"

#include <sys/wait.h> 
#include <string>
#include <iostream>

using namespace ::simgrid;


XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor::Actor(unsigned long int addr, std::thread::id pid) : p_remoteAddr(addr), pHost(NULL), pThreadId(pid) {
}


void rsg::Actor::kill() { 
  // ::kill(this->pPid, SIGKILL);

  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").kill(this->p_remoteAddr);
  
    /* in parent */
  // if (waitpid(this->pPid, NULL, 0) < 0) {
  //     perror("Failed to collect child process");
  // }
}

void rsg::this_actor::quit(void) {

  MultiThreadedSingletonFactory factory = MultiThreadedSingletonFactory::getInstance();
  ClientEngine& engine = factory.getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").close();
  factory.clearEngine(std::this_thread::get_id());

}

void rsg::this_actor::sleep(const double duration) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").sleep(duration);
}

void rsg::this_actor::execute(const double flops) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").execute(flops);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content, size_t dataSize) {
	rsg::this_actor::send(mailbox, content, dataSize, dataSize);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content,size_t dataSize, size_t simulatedSize) {
	std::string strContent(content, dataSize);
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").send(mailbox.getRemote(), strContent, simulatedSize);
}

char *rsg::this_actor::recv(Mailbox &mailbox) {
	std::string res;
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").recv(res, mailbox.getRemote());
	char *content = (char*) malloc(sizeof(char*) * res.length());
  memcpy(content, res.data(), res.length());
	return content;
}

char* rsg::Actor::getName() {
  std::string res;
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").getName(res, this->p_remoteAddr);
  char* res_cstr = (char*) malloc( (sizeof(char*) * res.length()) + 1);

  strcpy(res_cstr, res.c_str());
  return res_cstr;
}

rsg::Host* rsg::Actor::getHost() {
  rsgHostCurrentResType res;
  if(pHost == NULL) {
    ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
    engine.serviceClientFactory<RsgActorClient>("RsgActor").getHost(res, this->p_remoteAddr);
    pHost = new Host(res.name, res.addr);
  }
  return pHost;
}

int rsg::Actor::getPid() {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
   return  engine.serviceClientFactory<RsgActorClient>("RsgActor").getPid(this->p_remoteAddr);
}

void rsg::Actor::setAutoRestart(bool autorestart) {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").setAutoRestart(this->p_remoteAddr, autorestart);
}

void rsg::Actor::setKillTime(double time){
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").setKillTime(this->p_remoteAddr, time);
}

double rsg::Actor::getKillTime() {
  ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  return engine.serviceClientFactory<RsgActorClient>("RsgActor").getKillTime(this->p_remoteAddr);
}

void rsg::Actor::killAll() {
	ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  engine.serviceClientFactory<RsgActorClient>("RsgActor").killAll();
}

void actorRunner(std::function<int()> code, int port) {
  debug_process("Creating engine");
  MultiThreadedSingletonFactory::getInstance().getEngineOrCreate(std::this_thread::get_id(), port);
  debug_process("End of Create engine");
  try {
    debug_process("before code");
    code();
    debug_process("after codde  ");
  } catch(apache::thrift::TApplicationException &ex) {
    std::cerr<< "apache::thrift::TApplicationException in thread : " << ex.what() << std::endl;
  } catch(apache::thrift::transport::TTransportException &ex) {
    std::cerr<< "apache::thrift::transport::TTransportException in thread : " << ex.what() << std::endl;
  }
}

rsg::Actor *rsg::Actor::createActor(std::string name, rsg::Host host, std::function<int()> code) {
  debug_process("create actor");

	ClientEngine& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());
  rsgServerRemoteAddrAndPort params;
  debug_process("call prepare create actor");
  engine.serviceClientFactory<RsgActorClient>("RsgActor").createActorPrepare(params);
  debug_process("return from prepare create actor");

  std::thread first(actorRunner, code, params.port);     
  
  debug_process("call create actor");
  unsigned long int addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").createActor(params.addr, params.port ,name, host.p_remoteAddr, 10);
  debug_process("return from call create actor");

  rsg::Actor *act = new Actor(addr, first.get_id());
  first.detach();
  debug_process("end of create actor");
  return act;
}
