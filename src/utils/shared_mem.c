#include "shared_mem.h"

void* shared_mem_create_with_flags(const char* pathname, int proj_id, int size, int flags, int* shmid);

void* shared_mem_create(const char* pathname, int proj_id, int size, int* shmid) {
    return shared_mem_create_with_flags(pathname, proj_id, size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG | S_IRWXO, shmid);
}

void* shared_mem_connect(const char* pathname, int proj_id, int size) {
    return shared_mem_create_with_flags(pathname, proj_id, size, S_IRUSR | S_IWUSR, NULL);
}

void shared_mem_disconnect(const void* shmaddr) {
    if (shmdt(shmaddr) != 0) {
        log_warn("Error al desconectar de memoria.");
    }
}

void shared_mem_delete(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        log_warn("Error al eliminar memoria.");
    }
}

void* shared_mem_create_with_flags(const char* pathname, int proj_id, int size, int flags, int* shmid_out) {
    key_t shmkey;
    int shmid;
    void* shmaddr = NULL;

    if ((shmkey = ftok(pathname, proj_id)) == -1) {
        log_warn("Error en el ftok, al generar la clave del archivo %s.", pathname);
    }
    else {
        if ((shmid = shmget(shmkey, size, flags)) == -1) {
            log_warn("Error en el shmget, al obtener memoria de %d bytes.", size);
        }
        else {
            if ((shmaddr = shmat(shmid, NULL, 0)) == (void*) -1) {
                log_warn("Error en el shmat, no se pudo acceder a la memoria.");
                shmaddr = NULL;
            }
            else if (shmid_out != NULL) {
                *shmid_out = shmid;
            }
        }
    }

    return shmaddr;
}
