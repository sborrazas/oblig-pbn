#ifndef __SHARED_MEM_H__
#define __SHARED_MEM_H__

#include <sys/shm.h>
#include <sys/stat.h>
#include "debug.h"

void* create_shd_mem(const char* pathname, int proj_id, int size, int* shmaddr);

void* connect_shd_mem(const char* pathname, int proj_id, int size);

void disconnect_shd_mem(const void* shmaddr);

void delete_shd_mem(int shmid);

#endif
