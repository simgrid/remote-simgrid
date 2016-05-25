/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/actor.hpp"
#include "client/RsgClientEngine.hpp"

#include <string>
#include <iostream>

using namespace ::simgrid;

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor::Actor(unsigned long int addr) : p_remoteAddr(addr), pHost(NULL) {
}

//TODO Create dedicated function
void rsg::Actor::kill() { 
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").close();
	engine.close();
	ClientEngine::reset();
}

void rsg::this_actor::quit(void) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").close();
	engine.close();
	ClientEngine::reset();
}

void rsg::this_actor::sleep(const double duration) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").sleep(duration);
}

void rsg::this_actor::execute(const double flops) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").execute(flops);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content, size_t dataSize) {
	rsg::this_actor::send(mailbox, content, dataSize, dataSize);
}

void rsg::this_actor::send(rsg::Mailbox &mailbox, const char*content,size_t dataSize, size_t simulatedSize) {
	std::string strContent(content, dataSize);
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").send(mailbox.getRemote(), strContent, simulatedSize);
}

char *rsg::this_actor::recv(Mailbox &mailbox) {
	std::string res;
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").recv(res, mailbox.getRemote());
	char *content = (char*) malloc(sizeof(char*) * res.length());
  memcpy(content, res.data(), res.length());
	return content;
}

char* rsg::Actor::getName() {
  std::string res;
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").getName(res, this->p_remoteAddr);
  char* res_cstr = (char*) malloc( (sizeof(char*) * res.length()) + 1);

  strcpy(res_cstr, res.c_str());
  return res_cstr;
}

rsg::Host* rsg::Actor::getHost() {
  rsgHostCurrentResType res;
  if(pHost == NULL) {
    ClientEngine& engine = ClientEngine::getInstance();
    engine.serviceClientFactory<RsgActorClient>("RsgActor").getHost(res, this->p_remoteAddr);
    pHost = new Host(res.name, res.addr);
  }
  return pHost;
}

int rsg::Actor::getPid() {
   ClientEngine& engine = ClientEngine::getInstance();
   return  engine.serviceClientFactory<RsgActorClient>("RsgActor").getPid(this->p_remoteAddr);
}

void rsg::Actor::setAutoRestart(bool autorestart) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").setAutoRestart(this->p_remoteAddr, autorestart);
}

void rsg::Actor::setKillTime(double time){
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").setKillTime(this->p_remoteAddr, time);
}

double rsg::Actor::getKillTime() {
  ClientEngine& engine = ClientEngine::getInstance();
  return engine.serviceClientFactory<RsgActorClient>("RsgActor").getKillTime(this->p_remoteAddr);
}

void rsg::Actor::killAll() {
	ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").killAll();
}

rsg::Actor *rsg::Actor::createActor(std::string name, rsg::Host host, std::function<int()> code) {
	ClientEngine& engine = ClientEngine::getInstance();
	engine.close();
	
	if(fork()) {
		ClientEngine::reset();
		ClientEngine::getInstance();
		code();
	}
	
	engine.connect();
  unsigned long int addr = engine.serviceClientFactory<RsgActorClient>("RsgActor").createActor(name, host.p_remoteAddr, 10 );

	return new Actor(addr);
}
