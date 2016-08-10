#include "Socket.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <sys/poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <errno.h>

int createServerSocket(int port) {
    int resSocket;
    struct sockaddr_in server;
    
    //Create socket
    resSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (resSocket == -1) {
        printf("Could not create socket");
        return -1;
    }
    
    
    int enable = 1;
    if (setsockopt(resSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    puts("setsockopt(SO_REUSEADDR) failed");
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
    if(bind(resSocket ,(struct sockaddr *)&server , sizeof(server)) < 0)  {
        puts("bind error");
        return -1;
    }
    
    listen(resSocket , 3);
    
    return resSocket;
}

int socket_connect(const char *hostname, int port) {
    struct  hostent *hp;
    struct  sockaddr_in server;
    hp = gethostbyname(hostname);
    if (hp) {
        memset((void *) &server,0, sizeof(server));
        memmove((char *) &server.sin_addr, hp->h_addr, hp->h_length);
        server.sin_family = hp->h_addrtype;
        server.sin_port = (unsigned short) htons( port );
    } else {
        fprintf(stderr, "CLI: Local host not found. That's a serious DNS misconfiguration!\n");
    }
    
    /* create socket */
    int sock =  socket(AF_INET, SOCK_STREAM, 0);
    
    if (sock < 0) {
        perror("CLI: Cannot create a socket");
    }
    
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, 0, 0);
    
    /* connect to server */
    if (connect(sock, (const struct sockaddr*)&server, sizeof(server)) < 0) {
        fprintf(stderr, "%d: Cannot connect to local port %d\n",getpid(),port);
        exit(1);
    }
    
    return sock;
}

int getFreePort() {
    int rpcSocket; // we create a temporary socket
    
    rpcSocket = socket(AF_INET , SOCK_STREAM , 0);
    
    struct sockaddr_in rcp_server;
    rcp_server.sin_family = AF_INET;
    rcp_server.sin_addr.s_addr = INADDR_ANY;
    rcp_server.sin_port = 0;
    
    // Then we try bind it on port 0, as the funciton will return a free port.
    if(bind(rpcSocket ,(struct sockaddr *)&rcp_server , sizeof(rcp_server)) < 0)  {
        return -1;
        fprintf(stderr, "Cannot access to a free port\n");
    }
    
    struct sockaddr_in info;
    socklen_t restrict =  sizeof(info);
    getsockname(rpcSocket, (struct sockaddr *)&info, &restrict); // then we get the port given by bind
    return info.sin_port;
}

int isFreePort(int port) {
    int rpcSocket; // we create a temporary socket
    
    rpcSocket = socket(AF_INET , SOCK_STREAM , 0);
    
    struct sockaddr_in rcp_server;
    rcp_server.sin_family = AF_INET;
    rcp_server.sin_addr.s_addr = INADDR_ANY;
    rcp_server.sin_port = port;
    
    // Then we try bind it on port 0, as the funciton will return a free port.
    if(bind(rpcSocket ,(struct sockaddr *)&rcp_server , sizeof(rcp_server)) < 0)  {
        if(errno == EADDRINUSE) {
            printf("the port is not available. already to other process %d\n", port);
        } else {
            printf("could not bind to process (%d) %s\n", errno, strerror(errno));
        }
        return -1;
    }
    close(rpcSocket);
    return  1;
}

int getFreePort(unsigned int minValue) {
    unsigned int res = 0;
    do {
        res = getFreePort();
    } while(res < minValue);
    return res;
}
