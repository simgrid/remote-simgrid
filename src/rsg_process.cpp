/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "rsg/process.hpp"
#include "socket.h"
#include "command.h"

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_process, RSG, "RSG::Process");

namespace rsg = simgrid::rsg;

rsg::Process *rsg::Process::p_self = NULL;

rsg::Process::Process() {
	p_buffer_size = 4096;
	p_buffer = xbt_new(char,p_buffer_size);

	char *strport = getenv("RSG_PORT");
	if (strport == NULL)
		xbt_die("RSG_PORT not set. Did you launch this binary through rsg as expected?");
	int port = atoi(strport);

	p_sock = rsg_sock_connect(port);
}

rsg::Process &rsg::Process::self() {
	if (p_self == NULL) {
		p_self = new Process();
	}
	return *p_self;
}

void rsg::Process::sleep(double duration) {
	command_prepare(&p_buffer,&p_buffer_size, CMD_SLEEP, duration);
	exchange_data(p_sock, &p_buffer, &p_buffer_size);
	XBT_INFO("Answer of sleep cmd: >>%s<<",p_buffer);
}
