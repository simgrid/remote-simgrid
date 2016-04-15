#ifndef SRC_SOCKET_H_
#define SRC_SOCKET_H_

//Create a socket server which listen into the given port
int createServerSocket(int port);

// Helper funciton to get a free TCP port from the system.
// The specified minValue can garantie than we won't have a port under 1024.
int getFreePort(unsigned int minValue);

// Procedure running onto the client side to connect to the server socket.
int socket_connect(const char *, int port);

#endif
