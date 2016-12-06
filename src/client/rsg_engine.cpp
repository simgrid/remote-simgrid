/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/engine.hpp"
#include "RsgClient.hpp"
#include "../rsg/services.hpp"



using namespace ::simgrid;

double rsg::getClock() {
    return client->engine->getClock();
}
/*
void rsg::closeConnection() {
    client->engine->close();
}

int rsg::getRpcPort() {
    return client->engine->getRpcPort();
}

void rsg::setKeepAliveOnNextClientDisconnect(bool newValue) {
    client->engine->setKeepAliveOnNextClientDisconnect(newValue);
}
*/
