/* Copyright (c) 2006-2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/mailbox.hpp"
#include "client/RsgClientEngine.hpp"

using namespace ::simgrid;

XBT_LOG_EXTERNAL_CATEGORY(RSG);
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL,RSG,"RSG Communication Mailboxes");


rsg::Mailbox::Mailbox(const char*name, unsigned long int remoteAddr) {
	p_remoteAddr = remoteAddr;
	p_name=name;
}

rsg::Mailbox *rsg::Mailbox::byName(const char*name) {
	rsg::Mailbox * res;
	
	ClientEngine& engine = ClientEngine::getInstance();
	unsigned long int remoteAddr =  engine.serviceClientFactory<RsgMailboxClient>("RsgMailbox").mb_create(name);
		
	res = new Mailbox(name, remoteAddr);
		
	return res;
}

void rsg::Mailbox::shutdown() {
}
