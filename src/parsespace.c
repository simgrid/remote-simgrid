/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "rsg/parsespace.h"

rsg_parsespace_t *rsg_parsespace_new() {

	rsg_parsespace_t *res = xbt_new0(rsg_parsespace_t,1);
	res->buffer_size = 4096;
	res->buffer = xbt_new(char,res->buffer_size);

	return res;
}

void rsg_parsespace_free(rsg_parsespace_t *parsespace) {

	free(parsespace->buffer);
	free(parsespace->tokens);
	free(parsespace);
}
