/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <xbt.h>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "rsg/services.hpp"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"
#include "rsg/comm.hpp"

namespace simgrid  {
namespace rsg {

class Mailbox;
class Host;
class Comm;

class Actor {
	friend rsg::Comm;
private:
	Actor();
public:
	/** Retrieves an instance of your representative in the remote SimGrid world */
	static void killAll();
	static Actor *createActor(std::string name, rsg::Host host, std::function<int()> code);
	static void sleep(const double duration);
	static void send(Mailbox &mailbox,const char*content, int dataSize, int simulatedSize);
	static void send(Mailbox &mailbox,const char*content, int dataSize);
	static char *recv(Mailbox &mailbox);
	static void execute(const double flops);
	static void quit();
	
	void kill() {this->quit();}
	void setAutoRestart(bool autorestart);
	void setKillTime(double time);
	double getKillTime();
  const char*getName();
  Host *getHost();
  int getPid();
	~Actor() {}
private:
  rsg::Host *pHost;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_ACTOR_HPP */
