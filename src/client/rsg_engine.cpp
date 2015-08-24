/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "rsg/engine.hpp"
#include "xbt/ex.h"

#include "../rsg.pb.h"

namespace rsg = simgrid::rsg;

rsg::Engine *rsg::Engine::p_instance = NULL;
rsg::Engine *rsg::Engine::getInstance() {
	if (p_instance == NULL) {
		p_instance = new Engine();
		// Verify that the version of the library that we linked against is
		// compatible with the version of the headers we compiled against.
		GOOGLE_PROTOBUF_VERIFY_VERSION;
	}
	return p_instance;
}
