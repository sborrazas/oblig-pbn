#include <getopt.h>
#include "utils/debug.h"
#include "utils/random.h"
#include "utils/term.h"
#include "modules/queue_mem.h"
#include "modules/queue_process.h"

#define MAX_PROJ_ID 16777216
#define DEFAULT_MAX_MSGS 4
#define DEFAULT_MAX_ORIGS 4
#define DEFAULT_MAX_PROCS 2

static struct option queue_options[] = {
  {"msgs",  required_argument,  0, 'm'},
  {"origs", required_argument,  0, 'o'},
  {"procs", required_argument,  0, 'p'}
};

Queue_Mem* queue_mem;
int origin_server;
int processor_server;

int main(int argc, char* const argv[]) {
    int shmid;
    int proj_id;
    int max_msgs;
    int max_origs;
    int max_procs;

    proj_id = random_max(MAX_PROJ_ID);

    if ((max_msgs = term_int_option(argc, argv, queue_options, 0)) == -1) {
        log_warn("Opción `--msgs` no presente, utilizando el default de %d.", DEFAULT_MAX_MSGS);
        max_msgs = DEFAULT_MAX_MSGS;
    }
    if ((max_origs = term_int_option(argc, argv, queue_options, 1)) == -1) {
        log_warn("Opción `--origs` no presente, utilizando el default de %d.", DEFAULT_MAX_ORIGS);
        max_origs = DEFAULT_MAX_ORIGS;
    }
    if ((max_procs = term_int_option(argc, argv, queue_options, 2)) == -1) {
        log_warn("Opción `--procs` no presente, utilizando el default de %d.", DEFAULT_MAX_PROCS);
        max_procs = DEFAULT_MAX_PROCS;
    }

    printf("max_msg = %d\n", max_msgs);
    printf("max_origs = %d\n", max_origs);
    printf("max_procs = %d\n", max_procs);

    queue_mem = create_queue_mem(max_msgs, max_origs, max_procs, proj_id, &shmid);

    origin_server = fork_server("origin_server", proj_id);
    processor_server = fork_server("processor_server", proj_id);

    sleep(10000);

    delete_queue_mem(shmid);

    return 0;
}

/*
int callback() { // Se llama cuando un hijo se muere (SIGCHLD)
  int pid_hijo = wait();

  // Sacar hijo de mi lista de hijos
}
*/
