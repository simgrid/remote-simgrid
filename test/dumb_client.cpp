/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <stdio.h>
#include <rsg/actor.hpp>

using namespace simgrid::rsg;

int main(int argc, char **argv) {
	Actor *self = Actor::current();
	Mailbox *mbox = Mailbox::byName("toto");

	self->sleep(42);
	self->execute(8095000000); // That's the power of my host on the used platform

	self->send(*mbox,"message from client");
	char * msg = self->recv(*mbox);
	fprintf(stderr, "Client: Received message: '%s'\n",msg);
	free(msg);

	self->quit();
}
