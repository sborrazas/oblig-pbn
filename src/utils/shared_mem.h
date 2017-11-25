#ifndef __SHARED_MEM_H__
#define __SHARED_MEM_H__

#include <sys/shm.h>
#include <sys/stat.h>
#include "debug.h"

void* shared_mem_create(const char* pathname, int proj_id, int size, int* shmaddr);

void* shared_mem_connect(const char* pathname, int proj_id, int size);

void shared_mem_disconnect(const void* shmaddr);

void shared_mem_delete(int shmid);

#endif
