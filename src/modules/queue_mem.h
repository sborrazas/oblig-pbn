#ifndef __QUEUE_MEM_H__
#define __QUEUE_MEM_H__

#include "../utils/debug.h"
#include "../utils/shared_mem.h"

#define KEY_PATHNAME "/Users/sborrazas/hola"
// #define KEY_PATHNAME "/etc/msg_queue.conf"
#define PROJ_ID 123

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

typedef struct queue_mem {
  Message* hp_messages;
  Message* lp_messages;
  int      num_origins;
  Client*  origins;
  int      num_processors;
  Client*  processors;
} Queue_Mem;

Queue_Mem* create_queue_mem(int num_messages, int num_origins, int num_processors, int* shmid);

void delete_queue_mem(int shmid);

#endif
