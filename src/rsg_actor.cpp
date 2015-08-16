/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <rsg/actor.hpp>

#include "socket.h"
#include "rsg/parsespace.h"
#include "request.h"

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_ACTOR, RSG, "RSG::Actor");

namespace rsg = simgrid::rsg;

rsg::Actor *rsg::Actor::p_self = NULL;

rsg::Actor::Actor() {
	p_workspace = rsg_parsespace_new();

	char *strport = getenv("RSG_PORT");
	if (strport == NULL)
		xbt_die("RSG_PORT not set. Did you launch this binary through rsg as expected?");
	int port = atoi(strport);

	p_sock = rsg_sock_connect(port);
}

rsg::Actor &rsg::Actor::self() {
	if (p_self == NULL) {
		p_self = new Actor();
	}
	return *p_self;
}

void rsg::Actor::sleep(double duration) {
	rsg_request(p_sock, p_workspace, CMD_SLEEP, duration);
	XBT_INFO("Answer of sleep cmd: >>%s<<",p_workspace->buffer);
}

void rsg::Actor::execute(double flops) {
	rsg_request(p_sock, p_workspace, CMD_EXEC, flops);
	XBT_INFO("Answer of execute cmd: >>%s<<",p_workspace->buffer);
}

void rsg::Actor::quit(void) {
	rsg_request(p_sock, p_workspace, CMD_QUIT);
	XBT_INFO("Answer of quit cmd: >>%s<<",p_workspace->buffer);
	rsg_parsespace_free(p_workspace);
	p_workspace=NULL;
}
