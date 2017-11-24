#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include "debug.h"

#define MAX_WAITING_CONNECTIONS 10

int socket_create(int port);
int socket_accept(int listenfd);
void socket_close(int connfd);

int socket_send(int connfd, char* buffer, int size);
int socket_recv(int connfd, char* buffer, int size);

#endif
