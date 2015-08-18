/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef INCLUDE_RSG_PROTOCOL_H_
#define INCLUDE_RSG_PROTOCOL_H_

#include <xbt.h>

SG_BEGIN_DECL();

typedef enum {
	CMD_SLEEP = 0, CMD_EXEC, CMD_SEND, CMD_RECV,
	CMD_MB_CREAT,
	CMD_QUIT,
	CMD_COUNT /* Not a real command, just the sentinel to get the amount of commands */
} command_type_t;

SG_END_DECL();

#endif /* INCLUDE_RSG_PARSESPACE_H_ */
