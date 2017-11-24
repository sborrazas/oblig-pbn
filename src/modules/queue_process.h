#ifndef __QUEUE_PROCESS_H__
#define __QUEUE_PROCESS_H__

#include <stdio.h>
#include <unistd.h>

int fork_server(const char* servername, int proj_id);

int fork_controller(const char* controllername, int proj_id, int socket_fd);

#endif
