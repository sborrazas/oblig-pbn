#include <getopt.h>
#include <sys/wait.h>
#include "../utils/socket.h"
#include "../utils/debug.h"
#include "../utils/term.h"
#include "../utils/sig.h"
#include "queue_mem.h"
#include "queue_process.h"

#define PROCESSOR_SERVER_PORT 4000

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'}
};

static char* shortopts = "p:";

Queue_Mem* queue_mem;
int semid;

void handle_sigchld();
void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    int listen_fd;
    int conn_fd;
    short int is_active;
    int pid;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }

    if (signals_termination(handle_sigchld, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en origin_server.");
    }

    if ((queue_mem = queue_mem_connect(proj_id, &semid)) == NULL) {
        log_err("origin_server no pudo conectarse a shared_mem");
    }

    is_active = 1;
    listen_fd = socket_listen(PROCESSOR_SERVER_PORT);

    while (is_active) {
        if ((conn_fd = socket_accept(listen_fd)) == -1) {
            is_active = 0;
            log_err("Error al aceptar una nueva conexión.");
        }

        pid = fork_controller("build/modules/processor_controller", proj_id, conn_fd);

        log_info("Nueva conexión origin inicializada con pid = %d", pid);
    }

    return 0;
}

void handle_sigchld() {
    int pid;
    int status;

    while ((pid = waitpid(WAIT_ANY, &status, WNOHANG)) > 0) {
        log_info("SIGCHLD lanzada a processor_server con pid %d", pid);


        if (!queue_mem_remove_processor(queue_mem, semid)) {
            log_warn("Se intentó remover un procesador cuando hay 0.");
        }
    }
}

void handle_exit() {
    log_info("Terminando processor_server..");
    queue_mem_disconnect(queue_mem);
}
