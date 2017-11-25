#ifndef __QUEUE_PROCESS_H__
#define __QUEUE_PROCESS_H__

#include <stdio.h>
#include <unistd.h>
#include "../utils/debug.h"

int fork_server(const char* servername, int proj_id, int port);

int fork_controller(const char* controllername, int proj_id, int conn_fd);

#endif
