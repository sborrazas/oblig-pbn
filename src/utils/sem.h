#ifndef __SEM_H__
#define __SEM_H__

#include <sys/sem.h>
#include <sys/stat.h>
#include "debug.h"

#define SEM_GREEN 1
#define SEM_RED -1
#define SEM_INIT 0

union semum {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int sem_create(char* pathname, int proj_id, int count);
void sem_delete(int semid);
int sem_connect(char* pathname, int proj_id, int count);

void sem_p(int sem_num, int semid);
void sem_v(int sem_num, int semid);

#endif
