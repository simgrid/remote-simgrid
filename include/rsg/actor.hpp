/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <xbt.h>
#include <vector>

#include "rsg/RsgServiceImpl.h"

namespace simgrid {
namespace rsg {


class Actor {
private:
	Actor();
public:
	/** Retrieves an instance of your representative in the remote SimGrid world */
	static Actor &self();

	void quit();
	void sleep(double duration);

private:
	static Actor *pSelf;
	RsgServiceClient *pActorService;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_ACTOR_HPP */
