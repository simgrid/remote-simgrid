/* Copyright (c) 2006-2016. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/mailbox.hpp"
#include "client/RsgClient.hpp"
#include "client/multiThreadedSingletonFactory.hpp"

using namespace ::simgrid;

XBT_LOG_EXTERNAL_CATEGORY(RSG);
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL,RSG,"RSG Communication Mailboxes");


rsg::Mailbox::Mailbox(const char*name, unsigned long int remoteAddr) {
	p_remoteAddr = remoteAddr;
	p_name=name;
}

rsg::Mailbox *rsg::Mailbox::byName(const char*name) {
	rsg::Mailbox * res;
	
	Client& engine = MultiThreadedSingletonFactory::getInstance().getEngine(std::this_thread::get_id());

	try {
		unsigned long int remoteAddr =  engine.serviceClientFactory<RsgMailboxClient>("RsgMailbox").mb_create(name);
		res = new Mailbox(name, remoteAddr);
		return res;
	} catch(apache::thrift::transport::TTransportException &ex) {
		fprintf(stderr, "error creating mailbox %s in rsg::Mailbox::byName : %s \n", name ,ex.what());
		_exit(1);
	}
	return NULL;
}

void rsg::Mailbox::shutdown() {
}
