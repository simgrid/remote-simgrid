/* Copyright (c) 2006-2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/mailbox.hpp"
#include "client/RsgClientEngine.hpp"

XBT_LOG_EXTERNAL_CATEGORY(RSG);
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_CHANNEL,RSG,"RSG Communication Mailboxes");


using namespace simgrid;

boost::unordered_map <std::string, rsg::Mailbox *> *rsg::Mailbox::mailboxes = new boost::unordered_map<std::string, rsg::Mailbox*> ();
boost::shared_ptr<RsgMailboxClient> rsg::Mailbox::pMailboxService(NULL);

rsg::Mailbox::Mailbox(const char*name, unsigned long int remoteAddr) {
	p_remoteAddr = remoteAddr;
	p_name=name;
	mailboxes->insert({name, this});
}

rsg::Mailbox *rsg::Mailbox::byName(const char*name) {
	rsg::Mailbox * res;
	try {
		res = mailboxes->at(name);
	} catch (std::out_of_range& e) {
		if(!pMailboxService) {
			ClientEngine& engine = ClientEngine::getInstance();
			pMailboxService = boost::shared_ptr<RsgMailboxClient>(engine.serviceClientFactory<RsgMailboxClient>("RsgMailbox"));
		}
		unsigned long int remoteAddr = pMailboxService->mb_create(name);
		res = new Mailbox(name, remoteAddr);
	}
	return res;
}

void rsg::Mailbox::shutdown() {
	rsg::Mailbox::mailboxes->clear();
	delete rsg::Mailbox::mailboxes;
}
