/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#if 0 /* Don't fool the file indentation */
}
#endif
#endif

void tcp_recv(int sock, char **result, int *result_size);
void tcp_send(int sock, const char*data);

int rsg_createServerSocket(int port);
int rsg_sock_accept(int serverSocket);
int rsg_sock_connect(int port);

void exchange_data(int sock, const char*data, char **result, int *result_size);

#ifdef __cplusplus
#if 0 /* Don't fool the file indentation */
{
#endif
}
#endif


#endif /* SRC_SOCKET_H_ */
