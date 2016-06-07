/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <xbt.h>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <rsg/services.hpp>
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"
#include "rsg/comm.hpp"
#include <sys/types.h>
#include <thread>

namespace simgrid  {
namespace rsg {

class Mailbox;
class Host;
class Comm;

class Actor {
	friend rsg::Comm;
private:
	Actor(unsigned long int addr, std::thread::id  );
public:
	static void killAll();
	static Actor *createActor(std::string name, rsg::Host host, std::function<int(void *)> code, void *data);
	static void kill(int pid);
	void kill();
	void join();
	void setAutoRestart(bool autorestart);
	void setKillTime(double time);
	double getKillTime();
  char*getName();
  Host *getHost();
  int getPid();
	~Actor();

private:
	unsigned long int p_remoteAddr = 0;
	rsg::Host *pHost;
public:
		std::thread::id pThreadId;
};

namespace this_actor {

  // Static methods working on the current actor:

  /** Block the actor sleeping for that amount of seconds (may throws hostFailure) */
  XBT_PUBLIC(void) sleep(double duration);

  /** Block the actor, computing the given amount of flops */
  XBT_PUBLIC(void) execute(double flop);

  /** Block the actor until it gets a message from the given mailbox.
   *
   * See \ref Comm for the full communication API (including non blocking communications).
   */
  XBT_PUBLIC(char*) recv(Mailbox &chan);

  /** Block the actor until it delivers a message of the given simulated size to the given mailbox
   *
   * See \ref Comm for the full communication API (including non blocking communications).
  */	
	XBT_PUBLIC(void) send(Mailbox &mailbox, const char*content, size_t dataSize);

	XBT_PUBLIC(void) send(Mailbox &mailbox, const char*content, size_t dataSize, size_t simulatedSize);

	XBT_PUBLIC(void) quit();


};

}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_ACTOR_HPP */
