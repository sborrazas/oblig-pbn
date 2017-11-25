#include <getopt.h>
#include "../utils/socket.h"
#include "../utils/debug.h"
#include "../utils/term.h"
#include "queue_process.h"

#define ORIGIN_SERVER_PORT 3000

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'}
};

char* shortopts = "p:";

int main(int argc, char* const argv[]) {
    int proj_id;
    int listen_fd;
    int conn_fd;
    short int is_active;
    int pid;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }

    is_active = 1;
    listen_fd = socket_listen(ORIGIN_SERVER_PORT);

    while (is_active) {
        if ((conn_fd = socket_accept(listen_fd)) == -1) {
            is_active = 0;
            log_warn("Error al aceptar una nueva conexión.");
        }

        printf("Nueva conn_fd = %d", conn_fd);

        pid = fork_controller("build/modules/origin_controller", proj_id, conn_fd);

        // TODO: Handle pid, agregar cliente
    }

    return 0;
}
