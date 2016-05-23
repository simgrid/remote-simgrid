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

rsg::Actor::Actor() : pHost(NULL) {
}


void rsg::Actor::quit(void) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").close();
	engine.close();
	ClientEngine::reset();
}

void rsg::Actor::sleep(const double duration) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").sleep(duration);
}

void rsg::Actor::execute(const double flops) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").execute(flops);
}

void rsg::Actor::send(rsg::Mailbox &mailbox, const char*content, int dataSize) {
	rsg::Actor::send(mailbox, content,dataSize ,dataSize);
}

void rsg::Actor::send(rsg::Mailbox &mailbox, const char*content,int dataSize, int simulatedSize) {
	std::string strContent(content, dataSize);
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").send(mailbox.getRemote(), strContent, simulatedSize);
}

char *rsg::Actor::recv(Mailbox &mailbox) {
	std::string res;
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").recv(res, mailbox.getRemote());
	char *content = (char*) malloc(sizeof(char*) * res.length());
  memcpy(content, res.data(), res.length());
	return content;
}

const char* rsg::Actor::getName() {
  std::string res;
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").getName(res, 0);
  return res.c_str();
}

rsg::Host* rsg::Actor::getHost() {
  rsgHostCurrentResType res;
  if(pHost == NULL) {
    ClientEngine& engine = ClientEngine::getInstance();
    engine.serviceClientFactory<RsgActorClient>("RsgActor").getHost(res, 0);
    pHost = new Host(res.name, res.addr);
  }
  return pHost;
}

int rsg::Actor::getPid() {
   ClientEngine& engine = ClientEngine::getInstance();
   return  engine.serviceClientFactory<RsgActorClient>("RsgActor").getPid(0);
}

void rsg::Actor::setAutoRestart(bool autorestart) {
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").setAutoRestart(0, autorestart);
}

void rsg::Actor::setKillTime(double time){
  ClientEngine& engine = ClientEngine::getInstance();
  engine.serviceClientFactory<RsgActorClient>("RsgActor").setKillTime(0, time);
}

double rsg::Actor::getKillTime() {
  ClientEngine& engine = ClientEngine::getInstance();
  return engine.serviceClientFactory<RsgActorClient>("RsgActor").getKillTime(0);
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
	engine.serviceClientFactory<RsgActorClient>("RsgActor").createActor(name, host.p_remoteAddr, 10 );

	return NULL;
}
