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

void check_commands(void) {
	for (int i=0; i<CMD_COUNT; i++) {
		xbt_assert(commands[i].code == i, "command[] and command_type_t are out of sync at %d (for command %s).", i, commands[i].name);
	}
}

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


void command_prepare(char **buffer, int *buffer_size, command_type_t cmd, ...) {
	char *p = *buffer;
	int avail_size, incr;
	guarded_snprintf("{cmd:%s,",commands[cmd].name);

	va_list va;
	va_start(va,cmd);
	for (int it = 0; it<commands[cmd].argc; it++) {
		arg_t a = commands[cmd].args[it];
		guarded_snprintf("%s:",a.name);
		switch (a.fmt) {
		case 'd':
			guarded_snprintf("%d,",va_arg(va,int));
			break;
		case 'f':
			guarded_snprintf("%f,",va_arg(va,double));
			break;
		default:
			xbt_die("Unknown format %c for argument %d of cmd %s",a.fmt,it, commands[cmd].name);
		}
	}
	p--; // Remove the last ','
	guarded_snprintf("%c",'}'); /* Trick to pass an argument anyway */
}
#undef guarded_snprintf


static void json_tokenise(char *js, jsmntok_t **tokens, size_t *tok_count) {
    jsmn_parser parser;
    jsmn_init(&parser);

    int ret=JSMN_ERROR_NOMEM;
    if (*tok_count>0)
    	ret = jsmn_parse(&parser, js,strlen(js), *tokens, *tok_count);

    while (ret == JSMN_ERROR_NOMEM) {
        *tok_count = (*tok_count) * 2 + 1;
        *tokens = xbt_realloc(*tokens, sizeof(jsmntok_t) * (*tok_count));
        ret = jsmn_parse(&parser, js,strlen(js), *tokens, *tok_count);
    }

    if (ret == JSMN_ERROR_INVAL)
        xbt_die("jsmn_parse: invalid JSON string");
    if (ret == JSMN_ERROR_PART)
        xbt_die("jsmn_parse: truncated JSON string");
}
static int json_token_streq(char *js, jsmntok_t *t, const char *s) {
    return (strncmp(js + t->start, s, t->end - t->start) == 0
            && strlen(s) == (size_t) (t->end - t->start));
}
static char * json_token_tostr(char *js, jsmntok_t *t) {
    js[t->end] = '\0';
    return js + t->start;
}

/** Parse a jsoned command, and return the code of that command */
command_type_t command_identify(char *buffer, jsmntok_t **ptokens, size_t *tok_count) {
	json_tokenise(buffer, ptokens,tok_count);
	jsmntok_t *token = *ptokens;

	fprintf(stderr,"Got %ld tokens\n",*tok_count);

	xbt_assert(json_token_streq(buffer, &token[1],"cmd"), "First element of json is not 'cmd' but '%s'.",json_token_tostr(buffer, &token[1]));
	command_type_t cmd;
	for (cmd=0;cmd != CMD_COUNT && !json_token_streq(buffer,&token[2],commands[cmd].name); cmd ++) /* Nothing more to do */;
	xbt_assert(cmd != CMD_COUNT,"Command '%s' not known", json_token_tostr(buffer,&token[2]));


	return cmd;
}

/** Extract the args out of a jsoned command that was previously parsed (by command_identify) */
void command_getargs(char *buffer, jsmntok_t **ptokens, size_t *tok_count, command_type_t cmd, ...) {

	/* It was already parsed in command_identify */
	jsmntok_t *token = *ptokens;

	va_list va;
	va_start(va,cmd);
	for (int it = 0; it<commands[cmd].argc; it++) {
		arg_t a = commands[cmd].args[it];
		char *json_ctn = json_token_tostr(buffer, &token[it*2+4]);
		switch (a.fmt) {
		case 'd': {
			int *vd = va_arg(va, int*);
			*vd = atoi(json_ctn);
			break;
		}
		case 'f': {
			fprintf(stderr,"arg %d: %s\n",it,json_ctn);
			double *vf = va_arg(va,double*);
			*vf = strtod(json_ctn,NULL);
			break;
		}
		default:
			xbt_die("Unknown format %c for argument %d of cmd %s",a.fmt,it, commands[cmd].name);
		}
	}
}
