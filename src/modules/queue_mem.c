#include "queue_mem.h"

int queue_mem_size(int  max_messages, int max_origins, int max_processors);

Queue_Mem* create_queue_mem(int max_messages, int max_origins, int max_processors, int* shmid) {
    int size;
    Queue_Mem* queue_mem;

    size = queue_mem_size(max_messages, max_origins, max_processors);

    if ((queue_mem = (Queue_Mem*)create_shd_mem(KEY_PATHNAME, PROJ_ID, size, shmid)) == NULL) {
        log_err("No se pudo obtener la memoria compartida de tamaño %d bytes.", size);
    }

    queue_mem->num_origins = 0;
    queue_mem->num_processors = 0;

    queue_mem->hp_messages = (Message*)sizeof(Queue_Mem);
    queue_mem->lp_messages = queue_mem->hp_messages + sizeof(Message) * max_messages;

    queue_mem->origins = (Client*)queue_mem->hp_messages + sizeof(Message) * max_messages;
    queue_mem->processors = queue_mem->origins + sizeof(Client) * max_origins;

    return queue_mem;
}

void delete_queue_mem(int shmid) {
    delete_shd_mem(shmid);
}

int queue_mem_size(int max_messages, int max_origins, int max_processors) {
    return sizeof(Queue_Mem) +
      sizeof(Message) * 2 * max_messages +
      sizeof(Client) * (max_origins + max_processors);
}
