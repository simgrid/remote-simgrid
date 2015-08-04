/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <xbt.h>

#include <stdio.h>
#include <unistd.h>

#include "socket.h"

int main(int argc, char **argv) {
	char *strport = getenv("RSG_PORT");
	if (strport == NULL)
		xbt_die("This binary should be launched from rsg, not directly.");

	int port = atoi(strport);

	int sock = rsg_sock_connect(port);
	char *answer = NULL;
	int answer_size = 0;
	exchange_data(sock, "Bonjour, serveur!", &answer, &answer_size);

	fprintf(stderr, "%d: Got answer (len:%zd, size=%zd): '%s'\n", getpid(), strlen(answer), answer_size, answer);
}
