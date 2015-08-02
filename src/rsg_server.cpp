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

static int rsg_representative(int argc, char **argv) {

	XBT_INFO("Launching %s",argv[1]);

	if (! fork()) {
		// child. It will be blocked in the wait() on bash, but my father will continue.
		// Yup, that's an awful hack.
		putenv(bprintf("RSG_PORT=%d",serverPort));
		execl("/bin/sh", "sh", "-c", argv[1], (char *) 0);
	}
	int mysock = rsg_sock_accept(serverSocket);

	char buff[4096] = {0};
	read(mysock,buff,4096-1);
	XBT_INFO("Reading %s",buff);

	s4u::Process *self = s4u::Process::current();
	self->sleep(1);
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

	/* Create a server socket onto the forked applications will connect */
	serverSocket = rsg_createServerSocket(serverPort);

	/* Initialize the SimGrid world */
	e->loadPlatform(argv[1]);
	e->register_default(rsg_representative);
	e->loadDeployment("deploy.xml");
	e->run();
	XBT_INFO("Simulation done");
	close(serverPort);
	return 0;
}
