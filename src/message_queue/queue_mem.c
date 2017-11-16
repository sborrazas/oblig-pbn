#include "queue_mem.h"

Shared_Mem* init_shared_mem(int max_messages, int max_origins, int max_processors) {
    int size = shared_mem_size(max_messages, max_origins, max_processors);

    Queue_Mem* queue_mem = create_shd_mem(KEY_PATHNAME, PROJ_ID, size);

    queue_mem->num_origins = 0;
    queue_mem->num_processors = 0;

    shd_mem->hp_messages = sizeof(Queue_Mem);
    shd_mem->lp_messages = shd_mem->hp_messages + sizeof(Message) * max_messages;

    shd_mem->origins = shd_mem->hp_messages + sizeof(Message) * max_message;
    shd_mem->processors = shd_mem->origins + sizeof(Client) * max_origins;

    return queue_mem;
}

int shared_mem_size(int max_messages, int max_origins, int max_processors) {
  return sizeof(Queue_Mem) +
    sizeof(Message) * 2 * max_messages +
    sizeof(Client) * (max_origins + max_processors);
}
