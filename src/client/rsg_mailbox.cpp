/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>
#include <iostream>
#include "rsg/mailbox.hpp"
#include "RsgClient.hpp"
#include "multiThreadedSingletonFactory.hpp"

#include "../rsg/services.hpp"

using namespace ::simgrid;
using namespace ::RsgService;

XBT_LOG_EXTERNAL_CATEGORY(RSG);
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL,RSG,"RSG Communication Mailboxes");

rsg::Mailbox::Mailbox(const char*name, unsigned long int remoteAddr) {
	p_remoteAddr = remoteAddr;
	p_name=name;
}

rsg::MailboxPtr rsg::Mailbox::byName(const char*name) {
	rsg::MailboxPtr res;
	
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
	
	try {
		unsigned long int remoteAddr =  engine.serviceClientFactory<RsgService::RsgMailboxClient>("RsgMailbox").mb_create(name);
		res.reset(new Mailbox(name, remoteAddr));
		return res;
	} catch(apache::thrift::transport::TTransportException &ex) {
		fprintf(stderr, "error creating mailbox %s in rsg::Mailbox::byName : %s \n", name ,ex.what());
		_exit(1);
	}
	return NULL;
}

void rsg::Mailbox::shutdown() {
}

/** Declare that the specified process is a permanent receiver on that mailbox
*
* It means that the communications sent to this mailbox will start flowing to its host even before he does a recv().
* This models the real behavior of TCP and MPI communications, amongst other.
*/
void rsg::Mailbox::setReceiver(const rsg::Actor &process) {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
	engine.serviceClientFactory<RsgMailboxClient>("RsgMailbox").setReceiver(p_remoteAddr, process.p_remoteAddr);
}

/** Return the process declared as permanent receiver, or nullptr if none **/
rsg::Actor* rsg::Mailbox::receiver() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
	int64_t res = engine.serviceClientFactory<RsgMailboxClient>("RsgMailbox").getReceiver(p_remoteAddr);
	if(res == 0) {
		return NULL;
	}
	return new Actor(res,-1);
}

bool rsg::Mailbox::empty() {
	Client& engine = MultiThreadedSingletonFactory::getInstance().getClient(std::this_thread::get_id());
	return engine.serviceClientFactory<RsgMailboxClient>("RsgMailbox").empty(p_remoteAddr);
}