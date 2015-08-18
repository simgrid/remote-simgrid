/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <xbt.h>
#include <vector>

#include "rsg/parsespace.h"
#include "rsg/mailbox.hpp"

namespace simgrid {
namespace rsg {

class Mailbox;

class Actor {
	friend Mailbox;
private:
	Actor();
public:
	void sleep(double duration);
	void execute(double flops);
	void quit();
	void send(Mailbox *mailbox, const char*content);
	char *recv(Mailbox *mailbox);

	/** Retrieves an instance of your representative in the remote SimGrid world */
	static Actor &self();

protected:
	void request(int cmd, ...);

private:
	static Actor *p_self;

	/* To exchange data with the central server */
	int p_sock=-1;
	rsg_parsespace_t *p_workspace;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_ACTOR_HPP */
