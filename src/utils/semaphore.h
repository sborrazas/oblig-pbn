#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

#include <sys/sem.h>
#include <sys/stat.h>
#include "debug.h"

#define SEM_GREEN 1
#define SEM_RED -1
#define SEM_INIT 1

union semum {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int create_sem(char* pathname, int proj_id, int size);

void delete_sem(int sem_id);

void sem_p(int sem_num, int semid);

void sem_v(int sem_num, int semid);

#endif
