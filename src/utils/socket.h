#ifndef __SOCKET_H__
#define __SOCKET_H__

#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "debug.h"

#define MAX_WAITING_CONNECTIONS 10

int socket_listen(uint16_t port);
int socket_accept(int listen_fd);
int socket_connect(const char* address, uint16_t port);
void socket_close(int conn_fd);

int socket_send(int conn_fd, char* buffer, int size);
int socket_recv(int conn_fd, char* buffer, int size);

#endif
