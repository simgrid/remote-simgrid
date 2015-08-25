/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <xbt.h>
#include <vector>

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
	void send(Mailbox *mailbox, const char*content, int simulatedSize);
	char *recv(Mailbox *mailbox);

	/** Retrieves an instance of your representative in the remote SimGrid world */
	static Actor &self();

private:
	static Actor *p_self;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_ACTOR_HPP */
