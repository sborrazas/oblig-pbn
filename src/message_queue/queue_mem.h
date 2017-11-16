#ifndef __QUEUE_MEM_H__
#define __QUEUE_MEM_H__

#include "debug.h"
#include <stdlib.h>

#defone KEY_PATHNAME "/tmp/queue" // TODO: Nombre más lindo
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

void init_shared_mem(int num_messages, int num_origins, int num_processors);

#endif
