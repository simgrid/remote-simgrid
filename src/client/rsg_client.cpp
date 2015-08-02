/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include <xbt.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>


static int make_socket(const char*http_server, int port) {
	/* get host info by name :*/
	struct  hostent *hp;
	struct  sockaddr_in     server;

	if ((hp = gethostbyname( http_server )
	)) {
		memset((void *) &server,0, sizeof(server));
		memmove((char *) &server.sin_addr, hp->h_addr, hp->h_length);
		server.sin_family = hp->h_addrtype;
		server.sin_port = (unsigned short) htons( port );
	} else
		xbt_die("CLI: Host not found: %s:%d\n",http_server,port);

	/* create socket */
	int sock =  socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0)
		perror("CLI: Cannot create a socket");
	setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

	/* connect to server */
	if (connect(sock, (const sockaddr*)&server, sizeof(server)) < 0) {
		fprintf(stderr, "CLI: Cannot connect to %s:%d\n",http_server, port);
		exit(1);
	}

	return sock;
}

/*
 * read a line from file descriptor
 * returns the number of bytes read. negative if a read error occurred
 * before the end of line or the max.
 * carriage returns (CR) are ignored.
 *
 * (C) 1998 Laurent Demailly. (Http put/get mini lib)
 */
static int http_read_line (
		int fd,  /* file descriptor to read from */
		char *buffer, /* placeholder for data */
		int max /* max number of bytes to read */
) { /* not efficient on long lines (multiple unbuffered 1 char reads) */
	int n=0;
	while (n<max) {
		if (read(fd,buffer,1)!=1) {
			n= -n;
			break;
		}
		n++;
		if (*buffer=='\015') continue; /* ignore CR */
		if (*buffer=='\012') break;    /* LF is the separator */
		buffer++;
	}
	*buffer=0;
	return n;
}


static char *sock_POST(int sock, const char *url, const char*data) {
	if (strchr(data,' ') || strchr(data,'\t') || strchr(data,'\n'))
		xbt_die("Our protocol does not allow whitespaces in the data yet. hack hack...");

	char buff[1024] = {0};
	//	char *header = bprintf("PUT /%s HTTP/1.0\015\012Content-Length: %zd\015\012User-Agent: C RSG Client\015\012\015\012",url,strlen(data));
	char *header = bprintf("PUT /%s/%s HTTP/1.0\015\012User-Agent: C RSG Client\015\012\015\012",url,data);
	int hlgr=strlen(header);
	int ret;

	/* send header */
	if (write(sock,header,hlgr)!=hlgr)
		xbt_die("CLI: Cannot send the headers: %s", strerror(errno));
	free(header);

	/* send data */
	if (data) {
		if (write(sock,data,strlen(data)) != (ssize_t)strlen(data))
			xbt_die("CLI: Cannot send the data: %s", strerror(errno));
		fprintf(stderr, "CLI: Sent: %s(%s)\n",url,data);
	}

	/* Get the answer */
	if (http_read_line(sock, buff, 1024-1) <= 0)
		xbt_die("Error while reading the answer (buff='%s')",buff);
	fprintf(stderr, "CLI: Header of answer: %s\n",buff);

	int http_ver;
	ret = -1;
	if (sscanf(buff,"HTTP/1.%d %03d",&http_ver, &ret)<=1)
		xbt_die("CLI: Parse error on the answer's header.");

	if (ret != 200)
		xbt_die("CLI: The server answered code %d instead of 200. Bailing out\n",ret);

	int data_length=-1;

	while (1) {
		if (http_read_line(sock, buff, 1024-1) <= 0)
			xbt_die("Error while reading the answer");
		fprintf(stderr, "CLI: seen hdr: %s\n",buff);
		if (buff[0]=='\0') // Empty line: end of headers
			break;
		sscanf(buff,"Content-Length: %d",&data_length);
	}
	if (data_length<0)
		xbt_die("CLI: Server did not put any Content-Length in its answer's headers.");

	char *ans = xbt_new0(char,data_length);

	int done;
	for (done = 0; done < data_length;) {
		int step = read(sock,ans+done,data_length-done);
		if (step <= 0)
			xbt_die("CLI: Something went wrong while reading the answer from the server. %d chars so far (%s).", done, ans);
		done += step;
	}

	return ans;
}

int main(int argc, char **argv) {

	int sock = make_socket("127.0.0.1",9000);
	char *answer  = sock_POST(sock, "toto/tutu", "ze,data");

	fprintf(stderr, "CLI: answer (len:%zd): %s\n", strlen(answer), answer);
}
