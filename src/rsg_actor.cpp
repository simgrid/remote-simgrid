/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <rsg/actor.hpp>

#include "socket.h"
#include "command.h"
#include "rsg/parsespace.h"

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
	request_prepare(p_workspace, CMD_SLEEP, duration);
	exchange_data(p_sock, p_workspace);
	answer_parse(p_workspace,CMD_SLEEP);
	XBT_INFO("Answer of sleep cmd: >>%s<<",p_workspace->buffer);
}
