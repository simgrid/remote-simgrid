/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_RSG_ACTOR_HPP_
#define SRC_RSG_ACTOR_HPP_

#include <xbt.h>
#include <vector>

namespace simgrid {
namespace rsg {

class Actor {
private:
	Actor();
public:
	void sleep(double duration);

	/** Retrieves an instance of your representative in the remote SimGrid world */
	static Actor &self();

private:
	static Actor *p_self;

	/* To exchange data with the central server */
	int p_sock=-1;
	char *p_buffer;
	int p_buffer_size; // length of above buffer
	void *p_tokens = NULL; // To parse the json
	size_t p_tok_count = 0; // length of above array
};
}} // namespace simgrid::rsg

#endif /* SRC_RSG_ACTOR_HPP_ */
