/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

#include "socket.h"

int rsg_createServerSocket(int port) {
	int res;
	fprintf(stderr,"Create a RSG server on %d\n",port);
	if ((res = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Server: error socket");
		exit(1);
	}
	struct sockaddr_in *serv_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
	memset((char *) serv_addr, (char) 0, sizeof(struct sockaddr_in));

	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(port);
	serv_addr->sin_addr.s_addr = htonl(INADDR_ANY);

	int on = 1;
	if (setsockopt(res, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		perror("Server: error setsockopt");
		exit(1);
	}

	if (bind(res, (struct sockaddr *) serv_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("Server: error bind");
		exit(1);
	}
	if (listen(res, SOMAXCONN) < 0) {
		perror("Server: error listen");
		exit(1);
	}
	return res;
}

int rsg_sock_accept(int serverSocket) {
	int res;

	int clilen = sizeof(struct sockaddr_in);
	struct sockaddr_in *cli_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

	if ((res = accept(serverSocket, (struct sockaddr *) cli_addr, (socklen_t *) & clilen)) < 0) {
		perror("Server: error accepting real message");
		exit(1);
	}
	return res;
}
