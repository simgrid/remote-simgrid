/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#include "socket.hpp"

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
#include <fcntl.h>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>

#include <xbt.h>

/** Called by the server before running the simulation. All remote processes will connect to that socket */
int rsg_createServerSocket(int port) {
	int res;
	//fprintf(stderr,"%d: Create a RSG server on %d\n",getpid(),port);
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

	int flags = fcntl(res, F_GETFL, 0);
	xbt_assert(flags>=0,"fcntl(GET) on the server socket failed: %s", strerror(errno));
	flags = flags|O_NONBLOCK;
	xbt_assert(fcntl(res, F_SETFL, flags) == 0, "fcntl(SET) on the server socket failed: %s",strerror(errno));

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

/** Called regularly by the representatives on the server to see whether an unexpected remote process wants to connect to us.
 *
 * This will happen when a new thread is created in one of the remote process, demanding the creation of a corresponding representative
 */
void rsg_sock_maybeaccept(int serverSocket) {
	int res;
	int clilen = sizeof(struct sockaddr_in);
	struct sockaddr_in cli_addr;

	res = accept(serverSocket, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
	if (res < 0) {
		if (errno != EAGAIN && errno != EWOULDBLOCK) {
			perror("Server: error accepting real message");
			exit(1);
		}
		return; // Nobody knocking on our door
	}
	xbt_die("TODO: implement serving unexpected remotes, probably calling representative_loop() on the new socket");
}

/** Called by the representative on the server side on start to get the connection from the remote process that it will serve */
int rsg_sock_accept(int serverSocket) {
	int res;
	bool again = true;

	int clilen = sizeof(struct sockaddr_in);
	struct sockaddr_in *cli_addr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));

	while (again) {
		res = accept(serverSocket, (struct sockaddr *) cli_addr, (socklen_t *) & clilen);
		if ((res<0 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))) {
			xbt_os_sleep(0.01);
		} else {
			again=false;
		}
	}

	if (res < 0) {
		perror("Server: error accepting real message");
		exit(1);
	}
	//fprintf(stderr,"%d: accepted an incoming connection\n",getpid());
	free(cli_addr);
	return res;
}

/** Called by the remote process when using CPP to connect back to the server */
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
	//fprintf(stderr,"%d: Connected to port %d\n",getpid(),port);

	return sock;
}

/** Send the protobuff message on the socket, framing it correctly.
 * Used by both server and remote processes.
 * If you change this, you need to adapt the framing in the Java code, too.
 *
 * http://stackoverflow.com/questions/9496101/protocol-buffer-over-socket-in-c
 * http://stackoverflow.com/questions/2340730/are-there-c-equivalents-for-the-protocol-buffers-delimited-i-o-functions-in-ja
 * http://stackoverflow.com/questions/11640864/length-prefix-for-protobuf-messages-in-c
 * http://stackoverflow.com/questions/5670765/cannot-deserialize-protobuf-data-from-c-in-java
 *
 * http://java-performance.info/protobuf-data-encoding-for-numeric-datatypes/ (explains the wire protocol a bit)
 */
bool send_message(int socket, ::google::protobuf::Message *message)
{
  google::protobuf::uint32 message_length = message->ByteSize();
  int prefix_length = sizeof(message_length);
  int buffer_length = prefix_length + message_length;
  google::protobuf::uint8 buffer[buffer_length];
  memset(buffer, 0, buffer_length);

  google::protobuf::io::ArrayOutputStream array_output(buffer, buffer_length);
  google::protobuf::io::CodedOutputStream coded_output(&array_output);

  coded_output.WriteLittleEndian32(message_length);
  //fprintf(stderr,"send_message: %s (msg len: %d)\n",message->ShortDebugString().c_str(),message_length);
  message->SerializeToCodedStream(&coded_output);

  int sent_bytes = write(socket, buffer,   coded_output.ByteCount());
  if (sent_bytes != coded_output.ByteCount()) {
	  fprintf(stderr,"Sent only %d bytes out of the %d waiting",sent_bytes,coded_output.ByteCount());
	  return false;
  }

  return true;
}

/** Used by both server and remote processes */
bool recv_message(int socket, ::google::protobuf::Message *message)
{
  google::protobuf::uint32 message_length;
  int prefix_length = sizeof(message_length);
  google::protobuf::uint8 prefix[prefix_length];

  if (prefix_length != read(socket, prefix, prefix_length)) {
    return false;
  }
  google::protobuf::io::ArrayInputStream array_prefix(prefix, prefix_length);
  google::protobuf::io::CodedInputStream coded_prefix(&array_prefix);
  coded_prefix.ReadLittleEndian32(&message_length);
  xbt_assert (message_length < 64 * 1024*1024,
		  "Received a message of announced size %d. That's too much, giving up.", message_length);

  char buffer[message_length];
  size_t got;

  if (message_length != (got = read(socket, buffer, message_length))) {
	  fprintf(stderr,"Got %lu bytes instead of the %d expected ones\n",got, message_length);
      return false;
  }
  //fprintf(stderr,"I got the %d bytes that I was expecting\n",got);
  google::protobuf::io::ArrayInputStream array_input(buffer, message_length);
  google::protobuf::io::CodedInputStream coded_input(&array_input);

  if (!message->ParseFromCodedStream(&coded_input)) {
    return false;
  }
  //fprintf(stderr,"msg: %s\n",message->ShortDebugString().c_str());
  return true;
}

