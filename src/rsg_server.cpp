/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "xbt.h"
#include "simgrid/s4u.h"
#include <stdlib.h>

using namespace simgrid;

XBT_LOG_NEW_DEFAULT_CATEGORY(rsg_server, "RSG server (Remote SimGrid)");

int serverSocket;
int serverPort;
#include "socket.h"
#include "request.h"

static int rsg_representative(int argc, char **argv) {

	XBT_INFO("Launching %s",argv[1]);

	if (! fork()) {
		// child. I'm not in the mood of parsing the command line, so have bash do it for me.
		putenv(bprintf("RSG_PORT=%d",serverPort));
		execl("/bin/sh", "sh", "-c", argv[1], (char *) 0);
	}
	int mysock = rsg_sock_accept(serverSocket);

	s4u::Actor *self = s4u::Actor::current();
	rsg_parsespace_t *parsespace = rsg_parsespace_new();

	XBT_INFO("%d: Wait for incoming data",getpid());
	tcp_recv(mysock, parsespace);
	XBT_INFO("%d: Reading %s (len:%ld, size:%ld)",getpid(), parsespace->buffer,strlen(parsespace->buffer),parsespace->buffer_size);

	command_type_t cmd = request_identify(parsespace);
	switch (cmd) {
	case CMD_SLEEP: {
		double duration;
		rsg_request_getargs(parsespace, cmd, &duration);
		XBT_INFO("sleep(%f)",duration);
		self->sleep(duration);
		rsg_request_doanswer(mysock, parsespace,cmd);
		break;
	}
	case CMD_EXEC: {
		double flops;
		rsg_request_getargs(parsespace, cmd, &flops);
		XBT_INFO("execute(%f)",flops);
		self->execute(flops);
		rsg_request_doanswer(mysock, parsespace,cmd);
		break;
	}
	default:
		xbt_die("Received an unknown (but parsed!) command: %d %s",cmd,parsespace->buffer);
	}

	rsg_parsespace_free(parsespace);
	return 0;
}


int main(int argc, char **argv) {
	s4u::Engine *e = new s4u::Engine(&argc,argv);

	if (argc < 3) {
		fprintf(stderr,"Usage: rsg platform.xml port\n");
		exit(1);
	}
	XBT_INFO("argc: %d",argc);
	serverPort = atoi(argv[2]);
	if (serverPort < 1024)
		xbt_die("You should not run RSG on lower port %d.",serverPort);

	/* Create a server socket onto the forked applications will connect */
	serverSocket = rsg_createServerSocket(serverPort);

	/* Initialize the SimGrid world */
	e->loadPlatform(argv[1]);
	e->register_default(rsg_representative);
	e->loadDeployment("deploy.xml");
	e->run();
	XBT_INFO("Simulation done");
	close(serverPort);
	delete e;
	return 0;
}
