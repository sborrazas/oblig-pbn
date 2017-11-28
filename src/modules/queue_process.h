#ifndef __QUEUE_PROCESS_H__
#define __QUEUE_PROCESS_H__

#include <stdio.h>
#include <unistd.h>
#include "../utils/log.h"

int fork_server(const FILE* log_fp, const char* servername, int proj_id);

int fork_controller(const FILE* log_fp, const char* controllername, int proj_id, int conn_fd);

#endif
