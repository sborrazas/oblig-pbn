#ifndef __SHARED_MEM_H__
#define __SHARED_MEM_H__

void* create_shd_mem(char* pathname, int proj_id, int size);

void* connect_shd_mem(char* pathname, int proj_id, int size);

#endif
