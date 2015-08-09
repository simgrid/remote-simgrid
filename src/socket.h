/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

#include <xbt/misc.h>

SG_BEGIN_DECL();

void tcp_recv(int sock, char **result, int *result_size);
void tcp_send(int sock, const char*data);

int rsg_createServerSocket(int port);
int rsg_sock_accept(int serverSocket);
int rsg_sock_connect(int port);

/* Send the content of the buffer (nul-terminated), and receive the answer from the server.
 * The data may get reallocated if it's too small to receive the answer.
 */
void exchange_data(int sock, char **data, int *data_size);

SG_END_DECL();

#endif /* SRC_SOCKET_H_ */
