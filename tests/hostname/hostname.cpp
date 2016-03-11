/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <stdio.h>
#include <rsg/actor.hpp>
#include <rsg/host.hpp>

using namespace simgrid::rsg;

int main(int argc, char **argv) {
	Actor *self = Actor::current();
	Host *host = Host::current();
	fprintf(stderr,"hostname --> %s\n", host->name());
	self->quit();
}
