#include <getopt.h>
#include "../utils/debug.h"
#include "../utils/socket.h"
#include "../utils/term.h"
#include "../utils/sig.h"
#include "queue_mem.h"
#include "queue_process.h"

#define ORIGIN_SERVER_PORT 3000

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'}
};

static char* shortopts = "p:";

Queue_Mem* queue_mem;

void handle_sigchld();
void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    int listen_fd;
    int conn_fd;
    short int is_active;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }

    if (signals_termination(handle_sigchld, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en origin_server.");
    }

    if ((queue_mem = queue_mem_connect(proj_id)) == NULL) {
        log_err("origin_server no pudo conectarse a shared_mem");
    }

    is_active = 1;
    listen_fd = socket_listen(ORIGIN_SERVER_PORT);

    while (is_active) {
        if ((conn_fd = socket_accept(listen_fd)) == -1) {
            is_active = 0;
            log_err("Error al aceptar una nueva conexión.");
        }

        printf("Nueva conn_fd = %d", conn_fd);

        fork_controller("build/modules/origin_controller", proj_id, conn_fd);
    }

    return 0;
}

void handle_sigchld() {
    int pid;
    int status;

    printf("SIGCHLD lanzada a origin_server\n");

    while ((pid = waitpid(WAIT_ANY, &status, WNOHANG)) != 0) {
        if (pid < 0) {
            log_err("Ocurrió un error al obtener pid de hijo.");
        }

        queue_mem_remove_origin(queue_mem);
    }
}

void handle_exit() {
    printf("Terminando origin_server.. ");
    // TODO: Kill remaining origins
    queue_mem_disconnect(queue_mem);
    printf("Terminado.\n");
}
