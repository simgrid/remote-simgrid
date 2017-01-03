/* Copyright (c) 2006-2016. The SimGrid Team.
* All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
* under the terms of the license (GNU LGPL) which comes with this package. */

#include <xbt/log.h>

#include "rsg/kvs.hpp"
#include "RsgClient.hpp"
#include "../rsg/services.hpp"

using namespace ::simgrid::rsg;

// double rsg::getClock() {
//     return client->engine->getClock();
// }

void kvs::get(std::string& _return, const std::string& key) {
  client->kvs->get(_return, key);
}

void kvs::remove(const std::string& key) {
  client->kvs->remove(key);
}

void kvs::replace(const std::string& key, const std::string& data) {
  client->kvs->replace(key, data);
}

void kvs::insert(const std::string& key, const std::string& data) {
  client->kvs->insert(key, data);
}