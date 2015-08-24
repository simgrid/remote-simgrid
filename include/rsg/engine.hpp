/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_ENGINE_HPP_
#define SRC_RSG_ENGINE_HPP_

#include <xbt.h>
#include <vector>

namespace simgrid {
namespace rsg {

class Actor;
class Mailbox;

class Request;
class Answer;

class Engine {
	friend Actor;
	friend Mailbox;
private:
	Engine();

public:
	/** Retrieve your representative in the remote SimGrid world */
	static Engine &getInstance();

protected:
	/** Shutdown the stub after use. Done automatically on Actor::quit() */
	void shutdown();

	/* Must be called by any new thread that wants a simulation representative */
	// static Rsg *createProcess(); FIXME: TODO

protected:
	void sendRequest(rsg::Request &req, rsg::Answer &ans);

private:
	int p_sock = -1;
	static Engine *p_instance; // That class is a singleton in each remote application. FIXME: must be thread-local
};

}} // namespace simgrid::rsg

#endif /* SRC_RSG_ENGINE_HPP_ */
