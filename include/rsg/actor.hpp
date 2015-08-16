/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_ACTOR_HPP_
#define SRC_RSG_ACTOR_HPP_

#include <rsg/parsespace.h>
#include <xbt.h>
#include <vector>

namespace simgrid {
namespace rsg {

class Actor {
private:
	Actor();
public:
	void sleep(double duration);
	void execute(double flops);
	void quit();

	/** Retrieves an instance of your representative in the remote SimGrid world */
	static Actor &self();

private:
	static Actor *p_self;

	/* To exchange data with the central server */
	int p_sock=-1;
	rsg_parsespace_t *p_workspace;
};
}} // namespace simgrid::rsg

#endif /* SRC_RSG_ACTOR_HPP_ */
