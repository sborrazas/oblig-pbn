#include "semaphore.h"

int create_sem(char *pathname, int proj_id, int size) {
    return create_sem_with_flags(pathname, proj_id, size, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG | S_IRWXO);
}

int use_sem(char *pathname, int proj_id, int size) {
    return create_sem_with_flags(pathname, proj_id, size, 0);
}

int create_sem_with_flags(char *pathname, int proj_id, int size, int flags) {
    key_t semkey;
    int semid = -1;
    union semum semset;

    if ((semkey = ftok(pathname, proj_id)) == -1) {
        log_warn("Error en el ftok, al generar la clave del archivo %s.", pathname);
    }
    else {
        if ((semid = semget(semkey, size, flags)) == -1) {
            log_warn("Error en el semget, al obtener %d semáforo(s).", size);
        }
        else {
            semset.val = SEM_INIT;
            if (semctl(semid, 0, SETALL, &semset) == -1) {
                log_warn("Error en el semctl, no se pudo inicializar el semáforo.");
                semid = -1;
            }
        }
    }

    return semid;
}

void delete_sem(int semid) {
    if (semctl(semid, 0, IPC_RMID) == -1){
        log_warn("Error en el semctl, no se pudo eliminar el semáforo.");
    }
}

void sem_p(int sem_num, int semid) {
    struct sembuf set;
    set.sem_num = sem_num;
    set.sem_op = SEM_RED;
    set.sem_flg = 0;

    if (semop(semid, &set, 1) == -1) {
        log_warn("Error en el semop, no se pudo decrementar el semáforo %d.", sem_num);
    }
}

void sem_v(int sem_num, int semid) {
    struct sembuf set;
    set.sem_num = sem_num;
    set.sem_op = SEM_GREEN;
    set.sem_flg = 0;

    if (semop(semid, &set, 1) == -1) {
        log_warn("Error en el semop, no se pudo incrementar el semáforo %d.", sem_num);
    }
}
