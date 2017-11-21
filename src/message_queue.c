#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utils/debug.h"
#include "modules/queue_mem.h"
#include "utils/random.h"

#define MAX_PROJ_ID 16777216

int main(int argc, const char* argv[]) {
    int shmid;
    int proj_id = random_max(MAX_PROJ_ID);

    create_queue_mem(10, 10, 10, proj_id, &shmid);

    delete_queue_mem(shmid);

    return 0;
}

/*
int callback() { // Se llama cuando un hijo se muere (SIGCHLD)
  int pid_hijo = wait();

  // Sacar hijo de mi lista de hijos
}
*/
