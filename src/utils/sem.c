#include "sem.h"

int sem_create_with_flags(char *pathname, int proj_id, int count, int flags, short int initialize);

int sem_create(char *pathname, int proj_id, int count) {
    return sem_create_with_flags(pathname, proj_id, count, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG | S_IRWXO, 1);
}

int sem_connect(char *pathname, int proj_id, int count) {
    return sem_create_with_flags(pathname, proj_id, count, 0, 0);
}

int sem_create_with_flags(char *pathname, int proj_id, int count, int flags, short int initialize) {
    key_t semkey;
    int semid = -1;
    union semum semset;

    if ((semkey = ftok(pathname, proj_id)) == -1) {
        log_warn("Error en el ftok, al generar la clave del archivo %s.", pathname);
    }
    else {
        if ((semid = semget(semkey, count, flags)) == -1) {
            log_warn("Error en el semget, al obtener %d semáforo(s).", count);
        }
        else if (initialize) {
            semset.val = SEM_INIT;
            if (semctl(semid, 0, SETALL, &semset) == -1) { // Set all semaphores to SET_INIT (0)
                log_warn("Error en el semctl, no se pudo inicializar el semáforo.");
                semid = -1;
            }
        }
    }

    return semid;
}

void sem_delete(int semid) {
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
