/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <stdio.h>
#include <rsg/actor.hpp>
#include <rsg/host.hpp>
#include <xbt.h>

XBT_LOG_NEW_CATEGORY(RSG_TEST,"Test");
XBT_LOG_NEW_DEFAULT_SUBCATEGORY(RSG_TEST_OUTPUT, RSG_TEST, "RSG Test (Remote SimGrid)");

using namespace simgrid::rsg;

int main(int argc, char **argv) {
	Actor &self = Actor::self();
	Host &host = Host::current();
	XBT_INFO("CURRENT NAME --> %s", host.name().c_str());
	Host &host1 = Host::by_name("host1");
	XBT_INFO("HOSTNAME -> %s", host1.name().c_str());
	double speed = host1.speed();
	XBT_INFO("GET SPEED HOST 1 -> %f", speed);
	XBT_INFO("GET SPEED CURRENT HOST -> %f", host.speed());

	self.quit();
}
