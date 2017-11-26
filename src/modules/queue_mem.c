#include "queue_mem.h"

short int is_queue_full(Queue_Mem* mem);
void increase_index(int* index, Queue_Mem* mem);

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
    queue_mem->max_messages = max_msg;

    queue_mem->hp_messages.count = 0;
    queue_mem->hp_messages.start_index = 0;
    queue_mem->hp_messages.end_index = 0;
    queue_mem->lp_messages.count = 0;
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
    sem_delete(semid);
}

/******************************************************************************/
/* MEM OPERATIONS *************************************************************/
/******************************************************************************/

short int queue_mem_add_origin(Queue_Mem* queue_mem, int semid) {
    short int result;

    sem_p(ORIGINS_SEMAPHORE, semid);
    if ((result = queue_mem->num_origins < queue_mem->max_origins)) {
        queue_mem->num_origins++;
    }
    sem_v(ORIGINS_SEMAPHORE, semid);

    return result;
}

void queue_mem_remove_origin(Queue_Mem* queue_mem, int semid) {
    short int result;

    sem_p(ORIGINS_SEMAPHORE, semid);
    if ((result = queue_mem->num_origins > 0)) {
        queue_mem->num_origins--;
    }
    sem_v(ORIGINS_SEMAPHORE, semid);

    if (!result) {
        log_warn("Se intentó remover un origen cuando hay 0.");
    }
}

short int queue_mem_add_processor(Queue_Mem* queue_mem, int semid) {
    short int result;

    sem_p(PROCESSORS_SEMAPHORE, semid);
    if ((result = queue_mem->num_processors < queue_mem->max_processors)) {
        queue_mem->num_processors++;
    }
    sem_v(PROCESSORS_SEMAPHORE, semid);

    return result;
}

void queue_mem_remove_processor(Queue_Mem* queue_mem, int semid) {
    short int result;

    sem_p(PROCESSORS_SEMAPHORE, semid);
    if ((result = queue_mem->num_processors > 0)) {
        queue_mem->num_processors--;
    }
    sem_v(PROCESSORS_SEMAPHORE, semid);

    if (!result) {
        log_warn("Se intentó remover un procesador cuando hay 0.");
    }
}

void queue_mem_add_msg(Queue_Mem* mem, int semid, Message* msg) {
    sem_p(BUFFER_SEMAPHORE, semid);

    if (is_queue_full(mem)) {
        if (msg->high_priority) {
            mem->hp_messages.messages[mem->hp_messages.end_index] = *msg;
            increase_index(&mem->hp_messages.end_index, mem);
            if (mem->lp_messages.count > 0) {
                increase_index(&mem->lp_messages.start_index, mem);
                mem->hp_messages.count++;
                mem->lp_messages.count--;
            }
        }
        else {
            mem->lp_messages.messages[mem->lp_messages.end_index] = *msg;
            increase_index(&mem->lp_messages.end_index, mem);
            increase_index(&mem->lp_messages.start_index, mem);
        }
        sem_v(BUFFER_SEMAPHORE, semid);
    }
    else {
        if (msg->high_priority) {
            mem->hp_messages.messages[mem->hp_messages.end_index] = *msg;
            increase_index(&mem->hp_messages.end_index, mem);
            mem->hp_messages.count++;
        }
        else {
            mem->lp_messages.messages[mem->lp_messages.end_index] = *msg;
            increase_index(&mem->lp_messages.end_index, mem);
            mem->lp_messages.count++;
        }

        sem_v(BUFFER_SEMAPHORE, semid);
        sem_v(FULL_SEMAPHORE, semid);
    }
}

void queue_mem_remove_msg(Queue_Mem* mem, int semid, Message* msg) {
    sem_p(FULL_SEMAPHORE, semid);
    sem_p(BUFFER_SEMAPHORE, semid);

    if (mem->hp_messages.count > 0) {
        *msg = mem->hp_messages.messages[mem->hp_messages.start_index];
        increase_index(&mem->hp_messages.start_index, mem);
        mem->hp_messages.count--;
    }
    else {
        *msg = mem->lp_messages.messages[mem->lp_messages.start_index];
        increase_index(&mem->lp_messages.start_index, mem);
        mem->lp_messages.count--;
    }

    sem_v(BUFFER_SEMAPHORE, semid);
}

short int is_queue_full(Queue_Mem* mem) {
    return (mem->hp_messages.count + mem->lp_messages.count) == mem->max_messages;
}

void increase_index(int* index, Queue_Mem* mem) {
    int queues_size = mem->max_messages;

    *index = (*index + 1) % queues_size;
}
