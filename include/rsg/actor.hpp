/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SIMGRID_RSG_ACTOR_HPP
#define SIMGRID_RSG_ACTOR_HPP

#include <xbt.h>
#include <vector>

#include "rsg/RsgServiceImpl.h"
#include "rsg/mailbox.hpp"
#include "rsg/host.hpp"

namespace simgrid  {
namespace rsg {

class Mailbox;
class Host;

class Actor {
private:
	Actor();
public:
	/** Retrieves an instance of your representative in the remote SimGrid world */
  static Actor &self();
  void kill() {this->quit();}
  void quit();
  void sleep(const double duration);
  void execute(const double flops);
  char *recv(Mailbox &mailbox);
  void send(Mailbox &mailbox, const char*content);
  void send(Mailbox &mailbox, const char*content, int simulatedSize);
  const char*getName();
  Host *getHost();
  int getPid();

private:
	static Actor *pSelf;
  rsg::Host *pHost;
	RsgActorClient *pActorService;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_ACTOR_HPP */
