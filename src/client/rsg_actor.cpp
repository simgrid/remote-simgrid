/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/actor.hpp"
#include "rsg/engine.hpp"

#include "../rsg.pb.h"


XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

namespace rsg = simgrid::rsg;

rsg::Actor *rsg::Actor::p_self = NULL;

rsg::Actor::Actor() {
}

rsg::Actor &rsg::Actor::self() {
	if (p_self == NULL) {
		p_self = new Actor();
		// Verify that the version of the library that we linked against is
		// compatible with the version of the headers we compiled against.
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}
	return *p_self;
}

void rsg::Actor::sleep(double duration) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_SLEEP);
	req.mutable_sleep()->set_duration(duration);

	Engine::getInstance().sendRequest(req, ans);
	ans.Clear();
}

void rsg::Actor::execute(double flops) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_EXEC);
	req.mutable_exec()->set_flops(flops);

	Engine::getInstance().sendRequest(req, ans);
	ans.Clear();
}

void rsg::Actor::send(Mailbox *mailbox, const char*content) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_SEND);
	req.mutable_send()->set_mbox(mailbox->getRemote());
	req.mutable_send()->set_content(content);

	Engine::getInstance().sendRequest(req, ans);
	ans.Clear();
}
char *rsg::Actor::recv(Mailbox *mailbox) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_RECV);
	req.mutable_recv()->set_mbox(mailbox->getRemote());

	Engine::getInstance().sendRequest(req, ans);
	char *content = xbt_strdup(ans.recv().content().c_str());
	ans.Clear();
	return content;
}

void rsg::Actor::quit(void) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_QUIT);
	Engine::getInstance().sendRequest(req,ans);
	ans.Clear();
	Engine::getInstance().shutdown();
	google::protobuf::ShutdownProtobufLibrary();
}
