/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef INCLUDE_RSG_PARSESPACE_H_
#define INCLUDE_RSG_PARSESPACE_H_

#include <xbt.h>

SG_BEGIN_DECL();

typedef struct {
	char *buffer;
	void *tokens;
	size_t buffer_size;
	size_t tok_count;
} rsg_parsespace_t;

rsg_parsespace_t *rsg_parsespace_new(void);
void rsg_parsespace_free(rsg_parsespace_t *space);


SG_END_DECL();

#endif /* INCLUDE_RSG_PARSESPACE_H_ */
