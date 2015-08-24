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
#include "rsg.pb.h"

static int rsg_representative(int argc, char **argv) {

	simgrid::rsg::Request request;
	XBT_VERB("Launching %s",argv[1]);

	if (! fork()) {
		// child. I'm not in the mood of parsing the command line, so have bash do it for me.
		putenv(bprintf("RSG_PORT=%d",serverPort));
		execl("/bin/sh", "sh", "-c", argv[1], (char *) 0);
	}
	int mysock = rsg_sock_accept(serverSocket);

	s4u::Actor *self = s4u::Actor::current();

	bool done = false;
	while (!done) {
		simgrid::rsg::Answer ans;
		ans.set_type(request.type());

		XBT_DEBUG("%d: Wait for incoming data",getpid());
		if (!request.ParseFromFileDescriptor(mysock))
			xbt_die("ParseFromFileDescriptor returned false");

		switch (request.type()) {
		case simgrid::rsg::CMD_SLEEP: {
			double duration = request.sleep().duration();
			XBT_INFO("sleep(%f)",duration);
			self->sleep(duration);
			break;
		}
		case simgrid::rsg::CMD_EXEC: {
			double flops = request.exec().flops();
			XBT_INFO("execute(%f)",flops);
			self->execute(flops);
			break;
		}
		case simgrid::rsg::CMD_SEND: {
			s4u::Mailbox *mbox = (s4u::Mailbox*)request.send().mbox();
			char *content = (char*)request.send().content().c_str();
			XBT_INFO("send(%s,%s)",mbox->getName(),content);
			self->send(*mbox, xbt_strdup(content), strlen(content));
			break;
		}
		case simgrid::rsg::CMD_RECV: {
			s4u::Mailbox *mbox = (s4u::Mailbox*)request.recv().mbox();
			char *content = (char*)self->recv(*mbox);
			XBT_INFO("recv(%s) ~> %s",mbox->getName(), content);
			ans.mutable_recv()->set_content((const char*)content);
			free(content);
			break;
		}
		case simgrid::rsg::CMD_MB_CREATE: {
			const char *name = request.mbcreate().name().c_str();
			s4u::Mailbox *mbox = s4u::Mailbox::byName(name);
			XBT_INFO("mailbox_create(%s) ~> %p",name,mbox);
			ans.mutable_mbcreate()->set_remoteaddr((google::protobuf::uint64)mbox);
			break;
		}
		case simgrid::rsg::CMD_QUIT: {
			XBT_INFO("quit()");
			done = true;
			break;
		}
		default:
			xbt_die("Received an unknown command: %d. Did you implement the answer of your command in %s?",
					request.type(),__FILE__);
		} // switch request->type()

		ans.set_clock(simgrid::s4u::Engine::getClock());
		ans.SerializePartialToFileDescriptor(mysock);
	}

	return 0;
}


int main(int argc, char **argv) {
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;


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

	// Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
