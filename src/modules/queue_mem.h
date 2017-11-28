#ifndef __QUEUE_MEM_H__
#define __QUEUE_MEM_H__

#include "../utils/log.h"
#include "../utils/shared_mem.h"
#include "../utils/sem.h"
#include "mq_proto.h"

#define KEY_PATHNAME "/etc/msg_queue.mem"
#define MAX_MSGS 10000
#define MEM_SIZE sizeof(Queue_Mem) + \
          sizeof(Message) * 2 * MAX_MSGS
#define SEMAPHORES_COUNT 4

#define BUFFER_SEMAPHORE 0
#define FULL_SEMAPHORE 1
#define ORIGINS_SEMAPHORE 2
#define PROCESSORS_SEMAPHORE 3

typedef struct message {
    char      orig_name[NAME_SIZE + 1];
    int       counter;
    short int high_priority;
    char      datetime[DATE_SIZE + 1];
} Message;

typedef struct circular_queue {
    Message  messages[MAX_MSGS];
    int      start_index;
    int      end_index;
    int      count;
} Circular_Queue;

typedef struct queue_mem {
    Circular_Queue hp_messages;
    Circular_Queue lp_messages;
    int            num_origins;
    int            max_origins;
    int            num_processors;
    int            max_processors;
    int            max_messages;
} Queue_Mem;

Queue_Mem* queue_mem_create(FILE* fp_log, int num_msg, int num_orig,
                            int num_proc, int proj_id,
                            int* shmid, int* semid);
Queue_Mem* queue_mem_connect(FILE* fp_log, int proj_id, int* semid);
void queue_mem_disconnect(Queue_Mem* mem);
void queue_mem_delete(int shmid, int semid);

short int queue_mem_add_origin(Queue_Mem* queue_mem, int semid);
short int queue_mem_remove_origin(Queue_Mem* mem, int semid);
void queue_mem_add_msg(Queue_Mem* mem, int semid, Message* msg);
void queue_mem_remove_msg(Queue_Mem* mem, int semid, Message* msg);

short int queue_mem_add_processor(Queue_Mem* queue_mem, int semid);
short int queue_mem_remove_processor(Queue_Mem* mem, int semid);

#endif
