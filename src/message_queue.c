#include <getopt.h>
#include <signal.h>
#include <sys/wait.h>
#include "utils/debug.h"
#include "utils/random.h"
#include "utils/term.h"
#include "utils/sig.h"
#include "utils/log.h"
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
  {"procs", required_argument,  0, 'p'},
  {"log_file", required_argument,  0, 'l'}
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
FILE* log_fd;

void handle_sigchld();
void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    int max_msgs;
    int max_origs;
    int max_procs;
    short int in_menu_loop;

    if ((log_filepath = term_str_option(argc, argv, queue_options, queue_shortopts, 3)) == -1) {
        print_err("Opción requerida `--log_file` no presente.");
    }
    if ((max_msgs = term_int_option(argc, argv, queue_options, queue_shortopts, 0)) == -1) {
        print_warn("Opción `--msgs` no presente, utilizando el default de %d.", DEFAULT_MAX_MSGS);
        max_msgs = DEFAULT_MAX_MSGS;
    }
    if ((max_origs = term_int_option(argc, argv, queue_options, queue_shortopts, 1)) == -1) {
        print_warn("Opción `--origs` no presente, utilizando el default de %d.", DEFAULT_MAX_ORIGS);
        max_origs = DEFAULT_MAX_ORIGS;
    }
    if ((max_procs = term_int_option(argc, argv, queue_options, queue_shortopts, 2)) == -1) {
        print_warn("Opción `--procs` no presente, utilizando el default de %d.", DEFAULT_MAX_PROCS);
        max_procs = DEFAULT_MAX_PROCS;
    }

    if ((log_fd = log_open(log_filepath)) == NULL) {
        print_err("Archivo de log `%s` no tiene permisos para ser abierto.",
                  log_filepath);
    }

    proj_id = random_max(MAX_PROJ_ID);

    log_info(log_fd, "Inicializando message_queue con max_msg = %d, max_origs = %d, "
             "max_procs = %d, proj_id = %d", max_msgs, max_origs, max_procs, proj_id);

    queue_mem_create(max_msgs, max_origs, max_procs, proj_id, &shmid, &semid);

    if (signals_termination(handle_sigchld, handle_exit) != 0) {
        log_err(log_fd, "No se pudo registrar señales correctamente en message_queue.");
    }

    origin_server_pid = fork_server("build/modules/origin_server",
                                    log_filepath, proj_id);
    log_info(log_fd, "origin_server inicializado con pid %d", origin_server_pid);
    processor_server_pid = fork_server("build/modules/processor_server",
                                       log_filepath, proj_id);
    log_info(log_fd, "processor_server inicializado con pid %d", processor_server_pid);

    in_menu_loop = 1;

    while (in_menu_loop) {
        switch (term_get_menu_option(menu_options, 3)) {
        case 0: // Entrada de programa terminada
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

    while ((pid = waitpid(WAIT_ANY, &status, WNOHANG)) > 0) {
        log_info("SIGCHLD lanzada a message_queue con pid %d", pid);

        if (pid == origin_server_pid) origin_server_pid = 0;
        if (pid == processor_server_pid) processor_server_pid = 0;
    }

    exit(EXIT_SUCCESS);
}

void handle_exit() {
    if (origin_server_pid != 0) {
        log_info(log_fp, "Matando origin_server con pid %d..", origin_server_pid);
        kill(origin_server_pid, SIGINT);
    }
    if (processor_server_pid != 0) {
        log_info(log_fp, "Matando processor_server con pid %d..", processor_server_pid);
        kill(processor_server_pid, SIGINT);
    }

    log_info(log_fp, "Terminando message_queue..");
    queue_mem_delete(shmid, semid);

    log_close(log_fp);
}
