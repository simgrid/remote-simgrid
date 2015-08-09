/* Copyright (c) 2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#ifndef SRC_COMMAND_H
#define SRC_COMMAND_H

#include <xbt/misc.h>
#include "jsmn.h"

SG_BEGIN_DECL();

typedef struct {
	const char *name;
	const char fmt;
} arg_t;
#define NOARG {NULL,'\0'}

typedef struct {
	int code;
	const char *name;
	int argc;
	arg_t args[6];
} command_t;

typedef enum {
	CMD_SLEEP = 0,
	CMD_COUNT /* Not a real command, just the sentinel to get the amount of commands */
} command_type_t;

//	{CMD_SLEEP, "sleep",1,{{"duration",'f'},NOARG,NOARG,NOARG,NOARG,NOARG}}

void check_commands(void);

void command_prepare(char **buffer, int *buffer_size, command_type_t cmd, ...);

command_type_t command_identify(char *buffer, jsmntok_t **ptokens, size_t *tok_count);
void command_getargs(char *buffer, jsmntok_t **ptokens, size_t *tok_count, command_type_t cmd, ...);

SG_END_DECL();
#endif /* SRC_COMMAND_H */
