/* Copyright (c) 2015. The SimGrid Team. All rights reserved.              */

/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero Licence (see in file LICENCE).        */

#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

#include <google/protobuf/message.h>

#include <xbt/misc.h>

SG_BEGIN_DECL();

int rsg_createServerSocket(int port);
int rsg_sock_accept(int serverSocket);
int rsg_sock_connect(int port);

bool recv_message(int socket, ::google::protobuf::Message *message);
bool send_message(int socket, ::google::protobuf::Message *message);

SG_END_DECL();

#endif /* SRC_SOCKET_H_ */
