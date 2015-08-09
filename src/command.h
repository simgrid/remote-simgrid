/* Copyright (c) 2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */


#ifndef SRC_COMMAND_H
#define SRC_COMMAND_H

#include <xbt/misc.h>

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
	CMD_SLEEP = 0
} command_type_t;

//	{CMD_SLEEP, "sleep",1,{{"duration",'f'},NOARG,NOARG,NOARG,NOARG,NOARG}}

void prepare_command(char **buffer, int *buffer_size, command_type_t cmd, ...);

SG_END_DECL();
#endif /* SRC_COMMAND_H */
