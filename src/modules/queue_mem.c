#include "queue_mem.h"

int queue_mem_size(int max_messages, int max_origins, int max_processors);

Queue_Mem* create_queue_mem(int max_msg, int max_orig, int max_proc, int proj_id, int* shmid) {
    int size;
    Queue_Mem* queue_mem;

    size = queue_mem_size(max_msg, max_orig, max_proc);

    if ((queue_mem = (Queue_Mem*)create_shd_mem(KEY_PATHNAME, proj_id, size, shmid)) == NULL) {
        log_err("No se pudo obtener la memoria compartida de tamaño %d bytes.", size);
    }

    queue_mem->num_origins = 0;
    queue_mem->num_processors = 0;

    queue_mem->hp_messages.messages = (Message*)sizeof(Queue_Mem);
    queue_mem->hp_messages.size = max_msg;
    queue_mem->hp_messages.start_index = 0;
    queue_mem->hp_messages.end_index = 0;
    queue_mem->lp_messages.messages = queue_mem->hp_messages.messages + sizeof(Message) * max_msg;
    queue_mem->lp_messages.size = max_msg;
    queue_mem->lp_messages.start_index = 0;
    queue_mem->lp_messages.end_index = 0;

    queue_mem->origins = (Client*)queue_mem->hp_messages.messages + sizeof(Message) * max_msg;
    queue_mem->processors = queue_mem->origins + sizeof(Client) * max_orig;

    return queue_mem;
}

void delete_queue_mem(int shmid) {
    delete_shd_mem(shmid);
}

int queue_mem_size(int max_msg, int max_orig, int max_proc) {
    return sizeof(Queue_Mem) +
      sizeof(Message) * 2 * max_msg +
      sizeof(Client) * (max_orig + max_proc);
}
