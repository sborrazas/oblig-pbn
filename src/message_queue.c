#include <getopt.h>
#include <signal.h>
#include <sys/wait.h>
#include "utils/debug.h"
#include "utils/random.h"
#include "utils/term.h"
#include "utils/sig.h"
#include "modules/queue_mem.h"
#include "modules/queue_process.h"

#define MAX_PROJ_ID 16777216
#define DEFAULT_MAX_MSGS 100
#define DEFAULT_MAX_ORIGS 20
#define DEFAULT_MAX_PROCS 20

static struct option queue_options[] = {
  {"msgs",  required_argument,  0, 'm'},
  {"origs", required_argument,  0, 'o'},
  {"procs", required_argument,  0, 'p'},
  {"log_file", required_argument, 0, 'l'}
};

static char* queue_shortopts = "m:o:p:";

static char* menu_options[] = {
    "Cantidad de procesadores disponibles",
    "Cantidad de datos siendo procesados",
    "Cantidad de datos de alta prioridad",
    "Cantidad de datos de baja prioridad",
    "Detalle de los datos en la lista de espera",
    "Salir"
};

int shmid;
int semid;
int origin_server_pid;
int processor_server_pid;
int origin_server_pid = 0;
int processor_server_pid = 0;

void handle_sigchld();
void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    int max_msgs;
    int max_origs;
    int max_procs;
    short int in_menu_loop;
    char* log_filepath;
    int i;
    int size;
    Queue_Mem* queue_mem;
    Message* pending_messages;

    if ((log_filepath = term_str_option(argc, argv, queue_options, queue_shortopts, 3)) == NULL) {
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

    if (log_open(log_filepath) == -1) {
        print_err("Archivo de log `%s` no tiene permisos para ser abierto.",
                  log_filepath);
    }

    proj_id = random_max(MAX_PROJ_ID);

    log_info("Inicializando message_queue con max_msg = %d, max_origs = %d, "
             "max_procs = %d, proj_id = %d", max_msgs, max_origs, max_procs, proj_id);

    queue_mem = queue_mem_create(max_msgs, max_origs, max_procs, proj_id, &shmid, &semid);

    if (signals_termination(handle_sigchld, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en message_queue.");
    }

    origin_server_pid = fork_server("build/modules/origin_server",
                                    log_filepath, proj_id);
    log_info("origin_server inicializado con pid %d", origin_server_pid);
    processor_server_pid = fork_server("build/modules/processor_server",
                                       log_filepath, proj_id);
    log_info("processor_server inicializado con pid %d", processor_server_pid);

    in_menu_loop = 1;

    while (in_menu_loop) {
        switch (term_get_menu_option(menu_options, 6)) {
        case 0: // Entrada del programa terminada
            in_menu_loop = 0;
            break;
        case 1: // Cantidad de procesadores disponibles
            printf(">> # procesadores disponibles: %d\n",
                   queue_mem_available_processors(queue_mem, semid));
            break;
        case 2: // Cantidad de datos siendo procesados
            printf(">> # datos siendo procesados: %d\n",
                   queue_mem_busy_processors(queue_mem, semid));
            break;
        case 3: // Cantidad de datos de alta prioridad
            printf(">> # mensajes de alta prioridad: %d\n",
                   queue_mem_messages_count(queue_mem, semid, 1));
            break;
        case 4: // Cantidad de datos de baja prioridad
            printf(">> # mensajes de baja prioridad: %d\n",
                   queue_mem_messages_count(queue_mem, semid, 0));
            break;
        case 5: // Detalle de los datos en la lista de espera
            pending_messages = queue_mem_messages(queue_mem, semid, &size);
            for (i = 0; i < size; i++) {
                printf("%d. %s %04d %s %s\n", i + 1,
                       pending_messages[i].orig_name,
                       pending_messages[i].counter,
                       pending_messages[i].high_priority ? "ALTA" : "BAJA",
                       pending_messages[i].datetime);
            }
            if (size == 0) {
                printf(">> No hay mensajes en la cola\n");
            }
            break;
        case 6: // Salir
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
        log_info("Matando origin_server con pid %d..", origin_server_pid);
        kill(origin_server_pid, SIGINT);
    }
    if (processor_server_pid != 0) {
        log_info("Matando processor_server con pid %d..", processor_server_pid);
        kill(processor_server_pid, SIGINT);
    }

    log_info("Terminando message_queue..");
    queue_mem_delete(shmid, semid);

    log_close();
    printf("Bye!\n");
}
