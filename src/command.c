/* Copyright (c) 2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <xbt/ex.h>
#include "command.h"

command_t commands[] = {
		{CMD_SLEEP, "sleep",1,{{"duration",'f'},NOARG,NOARG,NOARG,NOARG,NOARG}}
};


#define guarded_snprintf(fmt, arg)                                       \
	do {                                                                 \
		avail_size = *buffer_size - (p-*buffer);                         \
		incr = snprintf(p, avail_size, fmt,arg); /* Payload is here! */  \
                                                                         \
		if (incr >= avail_size) { /*overflow*/                           \
			char *oldbuff = *buffer;                                     \
			*buffer = (char*)realloc(*buffer, *buffer_size+1024);        \
			if (*buffer == NULL)                                         \
				xbt_die("Cannot get a buffer big enough for command '%s'"\
						" (asked for %d bytes, got NULL).",              \
						commands[cmd].name, *buffer_size+1024);          \
                                                                         \
			*buffer_size += 1024;                                        \
	        /*char *oldp = p; */                                         \
			p = *buffer + (p-oldbuff);                                   \
			/*fprintf(stderr,"Overflow (inc: %d, avail:%d). buff move: %p -> %p (%ld), p move: %p -> %p (%ld), buffsize=%d\n",\
					 incr, avail_size, oldbuff, *buffer, (*buffer-oldbuff), oldp, p, (p-oldp), *buffer_size);*/\
		} else {                                                         \
			p += incr;                                                   \
		    /*fprintf(stderr,"no overflow. >>%s<<\n",*buffer); */        \
		}                                                                \
	} while (incr >= avail_size)


void prepare_command(char **buffer, int *buffer_size, command_type_t cmd, ...) {
	char *p = *buffer;
	int avail_size, incr;
	guarded_snprintf("{cmd='%s',",commands[cmd].name);

	va_list va;
	va_start(va,cmd);
	for (int it = 0; it<commands[cmd].argc; it++) {
		arg_t a = commands[cmd].args[it];
		guarded_snprintf("%s=",a.name);
		switch (a.fmt) {
		case 'd':
			guarded_snprintf("'%d',",va_arg(va,int));
			break;
		case 'f':
			guarded_snprintf("'%f',",va_arg(va,double));
			break;
		default:
			xbt_die("Unknown format %c for argument %d of cmd %s",a.fmt,it, commands[cmd].name);
		}
	}
	p--; // Remove the last ','
	guarded_snprintf("%c",'}'); /* Trick to pass an argument anyway */
}
#undef guarded_snprintf
