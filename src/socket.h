/*
 * socket.h
 *
 *  Created on: 23 juil. 2015
 *      Author: mquinson
 */

#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

#ifdef __cplusplus
extern "C" {
#if 0 /* Don't fool the file indentation */
}
#endif
#endif


int rsg_createServerSocket(int port);
int rsg_sock_accept(int serverSocket);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif


#endif /* SRC_SOCKET_H_ */
