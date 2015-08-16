/* Copyright (c) 2015. The SimGrid Team.
 * All rights reserved.                                                     */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <xbt/ex.h>
#include "command.h"

#define NOARG {NULL,'\0'}
#define VOID '\0'
command_t commands[] = {
		{CMD_SLEEP, "sleep",1,{{"duration",'f'},NOARG,NOARG,NOARG,NOARG,NOARG},VOID}
};

void check_protocol(void) {
	for (int i=0; i<CMD_COUNT; i++) {
		xbt_assert(commands[i].code == i, "command[] and command_type_t are out of sync at %d (for command %s).", i, commands[i].name);
	}
}

#define guarded_snprintf(fmt, arg)                                       \
	do {                                                                 \
		avail_size = workspace->buffer_size - (p- (workspace->buffer));  \
		incr = snprintf(p, avail_size, fmt,arg); /* Payload is here! */  \
                                                                         \
		if (incr >= avail_size) { /*overflow*/                           \
			char *oldbuff = workspace->buffer;                           \
			workspace->buffer = (char*)realloc(workspace->buffer,        \
					                      (workspace->buffer_size)+1024);\
			if (workspace->buffer == NULL)                               \
				xbt_die("Cannot get a buffer big enough for command '%s'"\
						" (asked for %zd bytes, got NULL).",              \
						commands[cmd].name, (workspace->buffer_size)+1024);\
                                                                         \
			workspace->buffer_size += 1024;                              \
	        /*char *oldp = p; */                                         \
			p = workspace->buffer + (p-oldbuff);                         \
			/*fprintf(stderr,"Overflow (inc: %d, avail:%d). buff move: %p -> %p (%ld), p move: %p -> %p (%ld), buffsize=%d\n",\
					 incr, avail_size, oldbuff, workspace->buffer, (workspace->buffer-oldbuff), oldp, p, (p-oldp), workspace->buffer_size);*/\
		} else {                                                         \
			p += incr;                                                   \
		    /*fprintf(stderr,"no overflow. >>%s<<\n",*buffer); */        \
		}                                                                \
	} while (incr >= avail_size)


void request_prepare(rsg_parsespace_t *workspace, command_type_t cmd, ...) {
	char *p = workspace->buffer;
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
extern double NOW; // To change the time directly. I love such nasty hacks.

void answer_prepare(rsg_parsespace_t *workspace, command_type_t cmd, ...) {
	char *p = workspace->buffer;
	int avail_size, incr;
	guarded_snprintf("{ret:%s,",commands[cmd].name);
	guarded_snprintf("clock:%f,", NOW);

	if (commands[cmd].retfmt != VOID) {
		va_list va;
		va_start(va,cmd);

		switch (commands[cmd].retfmt) {
		case 'd':
			guarded_snprintf("retcode:%d,",va_arg(va,int));
			break;
		case 'f':
			guarded_snprintf("retcode:%f,",va_arg(va,double));
			break;
		default:
			xbt_die("Unknown format %c for return of cmd %s",commands[cmd].retfmt, commands[cmd].name);
		}
	}
	p--; // Remove the last ','
	guarded_snprintf("%c",'}'); /* Trick to pass an argument anyway */
}
#undef guarded_snprintf

/* **************************************************************************************** */

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
static int json_token_streq(rsg_parsespace_t *workspace, int num, const char *s) {
	jsmntok_t *t = &((jsmntok_t*)workspace->tokens)[num];
    return (strncmp(workspace->buffer + t->start, s, t->end - t->start) == 0
            && strlen(s) == (size_t) (t->end - t->start));
}
static char * json_token_tostr(rsg_parsespace_t *workspace, int num) {
	jsmntok_t *t = &((jsmntok_t*)workspace->tokens)[num];
    workspace->buffer[t->end] = '\0';
    return workspace->buffer + t->start;
}
static double json_token_todouble(rsg_parsespace_t *workspace, int num) {
	jsmntok_t *t = &((jsmntok_t*)workspace->tokens)[num];
	char *end;
	double res = strtod(workspace->buffer+ t->start, &end);
	if (end != workspace->buffer+t->end)
		xbt_die("Parse error: JSON token '%s' does not seem to be a double", json_token_tostr(workspace,num));
	return res;
}

/** Parse a jsoned command, and return the code of that command */
command_type_t request_identify(rsg_parsespace_t *workspace) {
	json_tokenise(workspace->buffer, (jsmntok_t**)&workspace->tokens, &workspace->tok_count);

	xbt_assert(json_token_streq(workspace, 1,"cmd"),
			"First element of json is not 'cmd' but '%s'.",json_token_tostr(workspace, 1));
	command_type_t cmd;
	for (cmd=0;cmd != CMD_COUNT && !json_token_streq(workspace,2,commands[cmd].name); cmd ++) /* Nothing more to do */;
	xbt_assert(cmd != CMD_COUNT,"Command '%s' not known", json_token_tostr(workspace,2));

	return cmd;
}

/** Extract the args out of a jsoned command that was previously parsed (by command_identify) */
void request_getargs(rsg_parsespace_t *workspace, command_type_t cmd, ...) {

	va_list va;
	va_start(va,cmd);
	for (int it = 0; it<commands[cmd].argc; it++) {
		arg_t a = commands[cmd].args[it];
		int pos = it*2+4;

		char *json_ctn = json_token_tostr(workspace, pos);
		switch (a.fmt) {
		case 'd': {
			int *vd = va_arg(va, int*);
			*vd = atoi(json_ctn);
			break;
		}
		case 'f': {
			double *vf = va_arg(va,double*);
			*vf = json_token_todouble(workspace, pos);
			break;
		}
		default:
			xbt_die("Unknown format %c for argument %d of cmd %s",a.fmt,it, commands[cmd].name);
		}
	}
}

/** Parse a jsoned command, and return the code of that command */
void answer_parse(rsg_parsespace_t *workspace, command_type_t cmd, ...) {
	json_tokenise(workspace->buffer, (jsmntok_t**)&workspace->tokens, &workspace->tok_count);

	xbt_assert(json_token_streq(workspace,1,"ret"),
			"First element of json is not 'ret' but '%s'.", json_token_tostr(workspace,1));
	xbt_assert(json_token_streq(workspace,2,commands[cmd].name),
			"It's not an answer to command %s but to command %s.", commands[cmd].name, json_token_tostr(workspace,2));
	xbt_assert(json_token_streq(workspace,3,"clock"),
			"Second element of json  is not 'clock' but '%s'.", json_token_tostr(workspace,3));
	NOW = json_token_todouble(workspace,4);

	xbt_assert(commands[cmd].retfmt == VOID, "Not implemented: commands returning something (%s)",
			commands[cmd].name);
}

