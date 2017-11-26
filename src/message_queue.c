#include <getopt.h>
#include <signal.h>
#include "utils/debug.h"
#include "utils/random.h"
#include "utils/term.h"
#include "utils/sig.h"
#include "modules/queue_mem.h"
#include "modules/queue_process.h"

#define MAX_PROJ_ID 16777216
#define DEFAULT_MAX_MSGS 4
#define DEFAULT_MAX_ORIGS 4
#define DEFAULT_MAX_PROCS 2
#define ORIG_PORT 3000
#define PROC_PORT 3000

static struct option queue_options[] = {
  {"msgs",  required_argument,  0, 'm'},
  {"origs", required_argument,  0, 'o'},
  {"procs", required_argument,  0, 'p'}
};

static char* queue_shortopts = "m:o:p:";

static char* menu_options[] = {
    "aaaaaaa",
    "bbbbbbb",
    "Salir"
};

int shmid;
int semid;
int origin_server_pid;
int processor_server_pid;

void handle_sigchld();
void handle_exit();

int main(int argc, char* const argv[]) {
    Queue_Mem* queue_mem;
    int proj_id;
    int max_msgs;
    int max_origs;
    int max_procs;
    short int in_menu_loop;

    if ((max_msgs = term_int_option(argc, argv, queue_options, queue_shortopts, 0)) == -1) {
        log_warn("Opción `--msgs` no presente, utilizando el default de %d.", DEFAULT_MAX_MSGS);
        max_msgs = DEFAULT_MAX_MSGS;
    }
    if ((max_origs = term_int_option(argc, argv, queue_options, queue_shortopts, 1)) == -1) {
        log_warn("Opción `--origs` no presente, utilizando el default de %d.", DEFAULT_MAX_ORIGS);
        max_origs = DEFAULT_MAX_ORIGS;
    }
    if ((max_procs = term_int_option(argc, argv, queue_options, queue_shortopts, 2)) == -1) {
        log_warn("Opción `--procs` no presente, utilizando el default de %d.", DEFAULT_MAX_PROCS);
        max_procs = DEFAULT_MAX_PROCS;
    }

    proj_id = random_max(MAX_PROJ_ID);

    printf("max_msg = %d\n", max_msgs);
    printf("max_origs = %d\n", max_origs);
    printf("max_procs = %d\n", max_procs);
    printf("proj_id = %d\n", proj_id);

    queue_mem = queue_mem_create(max_msgs, max_origs, max_procs, proj_id, &shmid, &semid);

    if (signals_termination(handle_sigchld, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en message_queue.");
    }

    origin_server_pid = fork_server("build/modules/origin_server", proj_id);
    processor_server_pid = fork_server("build/modules/processor_server", proj_id);

    in_menu_loop = 1;

    while (in_menu_loop) {
        switch (term_get_menu_option(menu_options, 3)) {
        case 0:
            log_warn("Entrada de programa terminada.");
            in_menu_loop = 0;
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            in_menu_loop = 0;
            break;
        }
    }

    return 0;
}

void handle_sigchld() {
    int pid;
    int status;

    printf("SIGCHLD lanzada a message_queue\n");

    while ((pid = waitpid(WAIT_ANY, &status, WNOHANG)) != 0) {
        if (pid < 0) {
            log_err("Ocurrió un error al obtener pid de hijo.");
        }

        if (pid == origin_server_pid) origin_server_pid = 0;
        if (pid == processor_server_pid) processor_server_pid = 0;
    }

    exit(EXIT_SUCCESS);
}

void handle_exit() {
    if (origin_server_pid != 0) {
        printf("Terminando origin_server.. ");
        kill(origin_server_pid, SIGINT);
        printf("Terminado.\n");
    }
    if (processor_server_pid != 0) {
        printf("Terminando processor_server.. ");
        kill(processor_server_pid, SIGINT);
        printf("Terminado.\n");
    }

    printf("Terminando message_queue.. ");
    queue_mem_delete(shmid, semid);
    printf("Terminado.\n");
}
