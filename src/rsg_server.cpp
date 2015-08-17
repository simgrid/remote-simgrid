/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "xbt.h"
#include "simgrid/s4u.h"
#include <stdlib.h>

using namespace simgrid;

XBT_LOG_NEW_CATEGORY(RSG,"Remote SimGrid");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_SERVER, RSG, "RSG server (Remote SimGrid)");

int serverSocket;
int serverPort;
#include "rsg/socket.h"
#include "rsg/request.h"

static int rsg_representative(int argc, char **argv) {

	XBT_VERB("Launching %s",argv[1]);

	if (! fork()) {
		// child. I'm not in the mood of parsing the command line, so have bash do it for me.
		putenv(bprintf("RSG_PORT=%d",serverPort));
		execl("/bin/sh", "sh", "-c", argv[1], (char *) 0);
	}
	int mysock = rsg_sock_accept(serverSocket);

	s4u::Actor *self = s4u::Actor::current();
	rsg_parsespace_t *parsespace = rsg_parsespace_new();

	bool done = false;
	while (!done) {
		XBT_DEBUG("%d: Wait for incoming data",getpid());
		tcp_recv(mysock, parsespace);
		XBT_VERB("%d: Reading %s (len:%ld, size:%ld)",getpid(), parsespace->buffer,strlen(parsespace->buffer),parsespace->buffer_size);

		command_type_t cmd = rsg_request_identify(parsespace);
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
		case CMD_QUIT: {
			XBT_INFO("quit()");
			done = true;
			rsg_request_doanswer(mysock, parsespace,cmd);
			break;
		}
		case CMD_SEND: {
			char* mailbox, *content;
			rsg_request_getargs(parsespace, cmd, &mailbox, &content);
			XBT_INFO("send(%s,%s)",mailbox,content);
			self->send(*s4u::Mailbox::byName(mailbox), xbt_strdup(content), strlen(content));
			rsg_request_doanswer(mysock, parsespace,cmd);
			break;
		}
		case CMD_RECV: {
			char* mailbox;
			rsg_request_getargs(parsespace, cmd, &mailbox);
			char *content = (char*)self->recv(*s4u::Mailbox::byName(mailbox));
			XBT_INFO("recv(%s) ~> %s",mailbox, content);
			rsg_request_doanswer(mysock, parsespace,cmd, content);
			free(content);
			break;
		}
		default:
			xbt_die("Received an unknown (but parsed!) command: %d %s. Did you implement the answer of your command in %s?",
					cmd,parsespace->buffer,__FILE__);
		}
	}

	rsg_parsespace_free(parsespace);
	return 0;
}


int main(int argc, char **argv) {
	s4u::Engine *e = new s4u::Engine(&argc,argv);

	if (argc < 4) {
		fprintf(stderr,"Usage: rsg platform.xml deploy.xml port\n");
		exit(1);
	}
	XBT_INFO("argc: %d",argc);
	serverPort = atoi(argv[3]);
	if (serverPort < 1024)
		xbt_die("You should not run RSG on lower port %d.",serverPort);

	/* Create a server socket onto the forked applications will connect */
	serverSocket = rsg_createServerSocket(serverPort);

	/* Initialize the SimGrid world */
	e->loadPlatform(argv[1]);
	e->register_default(rsg_representative);
	e->loadDeployment(argv[2]);
	e->run();
	XBT_INFO("Simulation done");
	close(serverPort);
	delete e;
	return 0;
}
