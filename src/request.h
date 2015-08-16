/* Copyright (c) 2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#ifndef SRC_COMMAND_H
#define SRC_COMMAND_H

#include <rsg/parsespace.h>

#include "jsmn.h"

SG_BEGIN_DECL();

typedef struct {
	const char *name;
	const char fmt;
} arg_t;

typedef struct {
	int code;
	const char *name;
	int argc;
	arg_t args[6];
	char retfmt;
} command_t;

typedef enum {
	CMD_SLEEP = 0,
	CMD_COUNT /* Not a real command, just the sentinel to get the amount of commands */
} command_type_t;

//	{CMD_SLEEP, "sleep",1,{{"duration",'f'},NOARG,NOARG,NOARG,NOARG,NOARG}}

void check_protocol(void);

/* Prepare request on client side */
void rsg_request(int sock, rsg_parsespace_t *workspace, command_type_t cmd, ...);

/* Parse on server side */
command_type_t request_identify(rsg_parsespace_t *workspace);
void rsg_request_getargs(rsg_parsespace_t *workspace, command_type_t cmd, ...);

/* Prepare answer on server side */
void rsg_request_answer(int sock, rsg_parsespace_t *workspace, command_type_t cmd, ...);

SG_END_DECL();
#endif /* SRC_COMMAND_H */
