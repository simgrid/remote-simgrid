/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "unistd.h"

#include "rsg/engine.hpp"
#include "rsg/mailbox.hpp"
#include "../rsg/socket.hpp"
#include "../rsg.pb.h"

#include "xbt/ex.h"
#include <google/protobuf/stubs/common.h>


namespace rsg = simgrid::rsg;

rsg::Engine::Engine() {
	char *strport = getenv("RSG_PORT");
	if (strport == NULL)
		xbt_die("RSG_PORT not set. Did you launch this binary through rsg as expected?");
	int port = atoi(strport);
	p_sock = rsg_sock_connect(port);
}

void rsg::Engine::shutdown() {
	close(p_sock);
	rsg::Mailbox::shutdown();
}

rsg::Engine *rsg::Engine::p_instance = NULL;
rsg::Engine &rsg::Engine::getInstance() {
	if (p_instance == NULL) {
		p_instance = new Engine();
		// Verify that the version of the library that we linked against is
		// compatible with the version of the headers we compiled against.
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}
	return *p_instance;
}


extern double NOW;

void rsg::Engine::sendRequest(rsg::Request &req, rsg::Answer &ans) {
	//fprintf(stderr, "Actor sends a request %d: %s\n",req.type(),req.ShortDebugString().c_str());
	xbt_assert(send_message(p_sock, &req));
	req.Clear();

	xbt_assert(recv_message(p_sock, &ans));
	NOW = ans.clock();
}
