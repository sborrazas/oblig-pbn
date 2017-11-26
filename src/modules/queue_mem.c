#include "queue_mem.h"

short int is_queue_full(Queue_Mem* mem);

/******************************************************************************/
/* SARED_MEM ******************************************************************/
/******************************************************************************/

Queue_Mem* queue_mem_create(int max_msg, int max_orig, int max_proc, int proj_id, int* shmid, int* semid) {
    Queue_Mem* queue_mem;

    if ((queue_mem = (Queue_Mem*)shared_mem_create(KEY_PATHNAME, proj_id, MEM_SIZE, shmid)) == NULL) {
        log_err("No se pudo obtener la memoria compartida de tamaño %lu bytes.", MEM_SIZE);
    }
    if ((*semid = sem_create(KEY_PATHNAME, proj_id, SEMAPHORES_COUNT)) == -1) {
        shared_mem_delete(*shmid);
        log_err("No se pudo obtener los %d semáforos.", SEMAPHORES_COUNT);
    }

    queue_mem->num_origins = 0;
    queue_mem->max_origins = max_orig;
    queue_mem->num_processors = 0;
    queue_mem->max_processors = max_proc;

    queue_mem->hp_messages.messages = (Message*)sizeof(Queue_Mem);
    queue_mem->hp_messages.size = max_msg;
    queue_mem->hp_messages.start_index = 0;
    queue_mem->hp_messages.end_index = 0;
    queue_mem->lp_messages.messages = queue_mem->hp_messages.messages + sizeof(Message) * max_msg;
    queue_mem->lp_messages.size = max_msg;
    queue_mem->lp_messages.start_index = 0;
    queue_mem->lp_messages.end_index = 0;

    sem_v(BUFFER_SEMAPHORE, *semid);
    sem_v(ORIGINS_SEMAPHORE, *semid);
    sem_v(PROCESSORS_SEMAPHORE, *semid);

    return queue_mem;
}

Queue_Mem* queue_mem_connect(int proj_id, int* semid) {
    Queue_Mem* queue_mem;

    if ((queue_mem = shared_mem_connect(KEY_PATHNAME, proj_id, MEM_SIZE)) == NULL) {
        log_err("No se pudo conectar a la memoria compartida de tamaño %lu bytes.", MEM_SIZE);
    }
    if ((*semid = sem_connect(KEY_PATHNAME, proj_id, SEMAPHORES_COUNT)) == -1) {
        shared_mem_disconnect(queue_mem);
        log_err("No se pudo conectar para obtener los %d semáforos.", SEMAPHORES_COUNT);
    }

    return queue_mem;
}

void queue_mem_disconnect(Queue_Mem* mem) {
    shared_mem_disconnect(mem);
}

void queue_mem_delete(int shmid, int semid) {
    shared_mem_delete(shmid);
}

short int queue_mem_add_origin(Queue_Mem* queue_mem) {
    // TODO
}

void queue_mem_remove_origin(Queue_Mem* queue_mem) {
    // TODO
}

short int queue_mem_add_msg(Queue_Mem* mem, const char* orig_name, short int priority, int counter, const char* datetime) {
    return 1;
}
