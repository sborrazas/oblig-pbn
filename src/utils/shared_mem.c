#include "shared_mem.h"

void* create_shd_mem(char* pathname, int proj_id, int size) {
    return create_shd_mem_with_flags(pathname, proj_id, size, IPC_CREATE | IPC_EXCL);
}

void* connect_shd_mem(char* pathname, int proj_id, int size) {
    return create_shd_mem_with_flags(pathname, proj_id, 0);
}

void* create_shd_mem_with_flags(char* pathname, int proj_id, int flags) {
    key_t shmkey;
    int shmid;
    void* shmptr = NULL;

    if ((shmkey = ftok(pathname, proj_id)) == -1) {
        log_err(EXIT_SUCCESS, "Error en el ftok, al obtener la clave");
    }
    else {
        if ((shmid = shmget(shmkey, size, flags)) < 0) {
            log_err(EXIT_SUCCESS, "Error en el shmget, al obtener memoria");
        }
        else {
            if ((shmptr = shmat(shmid, NULL)) < -1) {
                log_err(EXIT_SUCCESS, "Error en el shmat, no pude acceder a la memoria");
                shmptr = NULL;
            }
        }
    }

    return shmptr;
}
