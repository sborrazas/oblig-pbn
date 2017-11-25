#ifndef __QUEUE_MEM_H__
#define __QUEUE_MEM_H__

#include "../utils/debug.h"
#include "../utils/shared_mem.h"

#define KEY_PATHNAME "/etc/msg_queue.conf"

#define NAME_SIZE 8
#define DATE_SIZE 23

typedef struct message {
    char      orig_name[NAME_SIZE];
    int       counter;
    short int priority;
    char      date[DATE_SIZE];
} Message;

typedef struct client {
    char name[NAME_SIZE];
    int  counter;
    // Datos conexión
} Client;

typedef struct circular_queue {
    Message* messages;
    int size;
    int start_index;
    int end_index;
} Circular_Queue;

typedef struct queue_mem {
    Circular_Queue hp_messages;
    Circular_Queue lp_messages;
    int      num_origins;
    Client*  origins;
    int      num_processors;
    Client*  processors;
} Queue_Mem;

Queue_Mem* queue_mem_create(int num_msg, int num_orig, int num_proc, int proj_id, int* shmid);

Queue_Mem* queue_mem_connect(int proj_id);

void queue_mem_delete(int shmid);

#endif
