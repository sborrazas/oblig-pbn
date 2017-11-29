#ifndef __QUEUE_PROCESS_H__
#define __QUEUE_PROCESS_H__

#include <stdio.h>
#include <unistd.h>
#include "../utils/debug.h"
#include "../utils/log.h"

int fork_server(const char* name, const char* logpath, int proj_id);

int fork_controller(const char* name, const char* logpath, int proj_id, int conn_fd);

#endif
