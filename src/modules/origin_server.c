#include <getopt.h>
#include "../utils/socket.h"
#include "../utils/debug.h"
#include "../utils/term.h"
#include "queue_process.h"

static struct option queue_options[] = {
  {"port",  required_argument,  0, 'm'},
  {"proj_id", required_argument,  0, 'o'}
};

int main(int argc, char* const argv[]) {
    int port;
    int proj_id;
    int listen_fd;
    int conn_fd;
    short int is_active;
    int pid;

    if ((port = term_int_option(argc, argv, queue_options, 0)) == -1) {
        log_err("Opción `--port` no presente. Finalizando..");
    }
    if ((proj_id = term_int_option(argc, argv, queue_options, 1)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }

    is_active = 1;
    listen_fd = socket_create(port);

    while (is_active) {
        if ((conn_fd = socket_accept(listen_fd)) == -1) {
            is_active = 0;
            log_warn("Error al aceptar una nueva conexión.");
        }

        pid = fork_controller("origin_controller", proj_id, conn_fd);

        // TODO: Handle pid, agregar cliente
    }

    return 0;
}
