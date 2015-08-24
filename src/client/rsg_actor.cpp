/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#include "rsg/parsespace.h"
#include "../rsg/socket.h"

#include "rsg/actor.hpp"
#include "../rsg.pb.h"

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

namespace rsg = simgrid::rsg;

rsg::Actor *rsg::Actor::p_self = NULL;

rsg::Actor::Actor() {
	char *strport = getenv("RSG_PORT");
	if (strport == NULL)
		xbt_die("RSG_PORT not set. Did you launch this binary through rsg as expected?");
	int port = atoi(strport);

	p_sock = rsg_sock_connect(port);
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

extern double NOW;

void sendRequest(int sock, rsg::Request &req, rsg::Answer &ans) {
	xbt_assert(req.SerializeToFileDescriptor(sock));

	xbt_assert(ans.ParseFromFileDescriptor(sock));
	NOW = ans.clock();
}

void rsg::Actor::sleep(double duration) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_SLEEP);
	req.mutable_sleep()->set_duration(duration);

	sendRequest(p_sock, req, ans);
}

void rsg::Actor::execute(double flops) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_EXEC);
	req.mutable_exec()->set_flops(flops);

	sendRequest(p_sock, req, ans);
}

void rsg::Actor::send(Mailbox *mailbox, const char*content) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_SEND);
	req.mutable_send()->set_mbox((google::protobuf::uint64)mailbox);
	req.mutable_send()->set_content(xbt_strdup(content));

	sendRequest(p_sock, req, ans);
}
char *rsg::Actor::recv(Mailbox *mailbox) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_RECV);
	req.mutable_recv()->set_mbox((google::protobuf::uint64)mailbox);

	sendRequest(p_sock, req, ans);
	char *content = (char*)ans.recv().content().c_str();
	return content;
}

void rsg::Actor::quit(void) {
	rsg::Request req;
	rsg::Answer ans;
	req.set_type(rsg::CMD_QUIT);
}
