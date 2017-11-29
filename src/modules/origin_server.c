#include <getopt.h>
#include <sys/wait.h>
#include "../utils/debug.h"
#include "../utils/socket.h"
#include "../utils/term.h"
#include "../utils/sig.h"
#include "queue_mem.h"
#include "queue_process.h"

#define ORIGIN_SERVER_PORT 3000

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'},
  {"log_file", required_argument, 0, 'l'}
};

static char* shortopts = "p:";

Queue_Mem* queue_mem;
int semid;

void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    int listen_fd;
    int conn_fd;
    short int is_active;
    char* log_filepath;
    int pid;

    if ((log_filepath = term_str_option(argc, argv, options, shortopts, 1)) == NULL) {
        print_err("Opción requerida `--log_file` no presente.");
    }
    if (log_open(log_filepath) == -1) {
        print_err("Archivo de log `%s` no tiene permisos para ser abierto.",
                  log_filepath);
    }
    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }

    if (signals_termination(NULL, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en origin_server.");
    }

    log_info("Inicializando origin_server con proj_id = %d", proj_id);

    if ((queue_mem = queue_mem_connect(proj_id, &semid)) == NULL) {
        log_err("origin_server no pudo conectarse a shared_mem");
    }

    is_active = 1;
    listen_fd = socket_listen(ORIGIN_SERVER_PORT);

    while (is_active) {
        if ((conn_fd = socket_accept(listen_fd)) == -1) {
            is_active = 0;
            log_err("Error al aceptar una nueva conexión.");
        }

        pid = fork_controller("build/modules/origin_controller", log_filepath, proj_id, conn_fd);

        log_info("Nueva conexión origin inicializada con pid = %d", pid);
    }

    return 0;
}

void handle_exit() {
    kill(0, SIGINT);
    log_info("Terminando origin_server..");
    queue_mem_disconnect(queue_mem);
}
