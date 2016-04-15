/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/actor.hpp"
#include "client/RsgClientEngine.hpp"


XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

rsg::Actor *rsg::Actor::pSelf = NULL;

rsg::Actor::Actor() {
	ClientEngine& engine = ClientEngine::getInstance();
	pActorService = engine.serviceClientFactory<RsgServiceClient>("rsgService");
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
