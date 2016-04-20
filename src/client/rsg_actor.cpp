/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/actor.hpp"
#include "client/RsgClientEngine.hpp"

#include <string>
#include <iostream>

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor *rsg::Actor::pSelf = NULL;

rsg::Actor::Actor() {
	ClientEngine& engine = ClientEngine::getInstance();
	pActorService = engine.serviceClientFactory<RsgServiceClient>("RsgService");
}

rsg::Actor &rsg::Actor::self() {
	if (pSelf == NULL) {
		pSelf = new Actor();
	}
	return *pSelf;
}

void rsg::Actor::quit(void) {
	pActorService->close();
}

void rsg::Actor::sleep(const double duration) {
	pActorService->sleep(duration);
}

void rsg::Actor::execute(const double flops) {
	pActorService->execute(flops);
}

void rsg::Actor::send(rsg::Mailbox &mailbox, const char*content) {
	this->send(mailbox,content, strlen(content)+1);
}

void rsg::Actor::send(rsg::Mailbox &mailbox, const char*content, int simulatedSize) {
	std::string strContent(content);
	pActorService->send(mailbox.getRemote(), strContent, simulatedSize);
}

char *rsg::Actor::recv(Mailbox &mailbox) {
	std::string res;
	pActorService->recv(res, mailbox.getRemote());
	char *content = xbt_strdup(res.c_str());
	return content;
}
