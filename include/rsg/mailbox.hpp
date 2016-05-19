/* Copyright (c) 2006-2015. The SimGrid Team. All rights reserved.          */

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

protected:
	static void shutdown(); /* clean all globals */
private:
	std::string p_name;
	unsigned long int p_remoteAddr = 0;
	static boost::unordered_map<std::string, Mailbox *> *mailboxes;
};
}} // namespace simgrid::rsg

#endif /* SIMGRID_RSG_MAILBOX_HPP */
