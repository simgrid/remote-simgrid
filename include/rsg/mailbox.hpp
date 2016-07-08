/* Copyright (c) 2006-2016. The SimGrid Team. All rights reserved.          */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#ifndef SIMGRID_RSG_MAILBOX_HPP
#define SIMGRID_RSG_MAILBOX_HPP

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

#include "rsg/services.hpp"
#include "actor.hpp"
#include "comm.hpp"

namespace simgrid {
namespace rsg {

class Comm;
class Actor;

/** @brief Mailboxes
 *
 * Rendez-vous point for network communications, similar to URLs on which you could post and retrieve data.
 * They are not network locations: you can post and retrieve on a given mailbox from anywhere on the network.
 * You can access any mailbox without any latency. The network delay are only related to the location of the
 * sender and receiver.
 */
class Mailbox {
	friend rsg::Comm;
private:
	Mailbox(const char*name, unsigned long int remoteAddr);
public:
	~Mailbox() {}

//protected:
	unsigned long int getRemote() { return p_remoteAddr; }
	const char * getName() { return p_name.c_str(); }
public:
	/** Retrieve the mailbox associated to the given string */
	static Mailbox *byName(const char *name);
	/** Declare that the specified process is a permanent receiver on that mailbox
   *
   * It means that the communications sent to this mailbox will start flowing to its host even before he does a recv().
   * This models the real behavior of TCP and MPI communications, amongst other.
   */
	//HACK If we passe an actor, the destructor of the copy will be detroy by the call.
	// Thus, the destructor will destruct the remote reference of the actor. Wich lead to an unstable state.
	// This is a hack because we should find a better way in order to handle the life cycle of rsg objects.
  void setReceiver(const rsg::Actor &process);
  /** Return the process declared as permanent receiver, or nullptr if none **/
  rsg::Actor* receiver();

protected:
	static void shutdown(); /* clean all globals */
private:
	std::string p_name;
	unsigned long int p_remoteAddr = 0;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_MAILBOX_HPP */
