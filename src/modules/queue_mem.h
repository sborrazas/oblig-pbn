#ifndef __QUEUE_MEM_H__
#define __QUEUE_MEM_H__

#include "../utils/debug.h"
#include "../utils/shared_mem.h"
#include "mq_proto.h"

#define KEY_PATHNAME "/etc/msg_queue.conf"
#define MAX_MSGS 10000
#define MEM_SIZE sizeof(Queue_Mem) + \
          sizeof(Message) * 2 * MAX_MSGS

typedef struct message {
    char      orig_name[NAME_SIZE + 1];
    int       counter;
    short int priority;
    char      date[DATE_SIZE + 1];
} Message;

typedef struct circular_queue {
    Message* messages;
    int      size;
    int      start_index;
    int      end_index;
} Circular_Queue;

typedef struct queue_mem {
    Circular_Queue hp_messages;
    Circular_Queue lp_messages;
    int            num_origins;
    int            max_origins;
    int            num_processors;
    int            max_processors;
} Queue_Mem;

Queue_Mem* queue_mem_create(int num_msg, int num_orig, int num_proc, int proj_id, int* shmid);
Queue_Mem* queue_mem_connect(int proj_id);
void queue_mem_disconnect(Queue_Mem* mem);
void queue_mem_delete(int shmid);

short int queue_mem_add_origin(Queue_Mem* queue_mem);
void queue_mem_remove_origin(Queue_Mem* mem);
short int queue_mem_add_msg(Queue_Mem* mem, const char* orig_name, short int priority, int counter, const char* datetime);

#endif
