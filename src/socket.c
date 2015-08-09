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

#include <xbt.h>

#include "socket.h"

int rsg_createServerSocket(int port) {
	int res;
	fprintf(stderr,"%d: Create a RSG server on %d\n",getpid(),port);
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
	free(serv_addr);
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
	//fprintf(stderr,"%d: accepted an incoming connection\n",getpid());
	free(cli_addr);
	return res;
}

int rsg_sock_connect(int port) {
	/* get host info by name :*/
	struct  hostent *hp;
	struct  sockaddr_in     server;

	if ((hp = gethostbyname( "127.0.0.1" )
	)) {
		memset((void *) &server,0, sizeof(server));
		memmove((char *) &server.sin_addr, hp->h_addr, hp->h_length);
		server.sin_family = hp->h_addrtype;
		server.sin_port = (unsigned short) htons( port );
	} else
		xbt_die("CLI: Local host not found. That's a serious DNS misconfiguration!\n");

	/* create socket */
	int sock =  socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
		perror("CLI: Cannot create a socket");
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

	/* connect to server */
	if (connect(sock, (const struct sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "%d: Cannot connect to local port %d\n",getpid(),port);
		exit(1);
	}
	fprintf(stderr,"%d: Connected to port %d\n",getpid(),port);

	return sock;
}

/* Send one chunk of data */
static void tcp_send_chunk(int sock, const char*data, int len) {
	unsigned long done=0;
	while (done < len) {
		int step = write(sock, data+done, len-done);
		if (step < 0)
			xbt_die("Problem while sending the data: %s", strerror(errno));
		done += step;
	}
}

static void tcp_recv_chunk(int sock, char*data,int len) {
	for (int done = 0; done < len;) {
		int step = read(sock,data+done,len-done);
		if (step <= 0)
			xbt_die("CLI: Something went wrong while reading the answer from the server. %d chars so far (%s).", done, data);
		done += step;
	}
	data[len]='\0';
}

/* Send one payload. First the size alone on its line, then the content */
void tcp_send(int sock, const char*data) {
	char buffer[128];
	int payload_len = strlen(data);
	sprintf(buffer,"%d\n",payload_len);
	tcp_send_chunk(sock, buffer, strlen(buffer));
	tcp_send_chunk(sock, data,   payload_len);
	//fprintf(stderr,"%d: Sent data >>%s<<\n", getpid(),data);
}

/** Receive one payload. First the size alone on its line, then the content.
 *
 * The result and result_size are allocated or extended on need, just like getline does.
 */
void tcp_recv(int sock, char **result, int *result_size) {
	char buffer[128];
	int payload_len;

	/* Read the answer length, as an integer alone on its line */
	int p = 0;
	while (read(sock, buffer+p, 1) >0 && buffer[p++] != '\n');
	buffer[p] = '\0';
	payload_len = atoi(buffer);

	/* Get ready to receive the answer */
	if (payload_len+1 > *result_size) {
		*result = (char*)realloc(*result, payload_len+1);
		*result_size = payload_len+1;
	}

	//fprintf(stderr,"%d: Wait for %d bytes\n",getpid(), payload_len);
	tcp_recv_chunk(sock, *result, payload_len);

}

void exchange_data(int sock, char **data, int *data_size) {

	tcp_send(sock, *data);
	//fprintf(stderr,"%d: Data sent. Wait for the answer.\n",getpid());

	tcp_recv(sock, data, data_size);
}


