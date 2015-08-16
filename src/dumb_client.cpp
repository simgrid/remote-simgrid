/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <rsg/actor.hpp>
#include <xbt.h>

#include <stdio.h>
#include <unistd.h>

#include "socket.h"

int main(int argc, char **argv) {
	simgrid::rsg::Actor &self = simgrid::rsg::Actor::self();

	self.execute(42000);
}
