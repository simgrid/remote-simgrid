/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <stdio.h>
#include <rsg/actor.hpp>

int main(int argc, char **argv) {
	simgrid::rsg::Actor &self = simgrid::rsg::Actor::self();
	simgrid::rsg::Mailbox *mbox = simgrid::rsg::Mailbox::byName("toto");

	char * msg = self.recv(mbox);
	fprintf(stderr, "Server: Received message: '%s'\n",msg);
	self.send(mbox, "Message from server");
	self.quit();
}
