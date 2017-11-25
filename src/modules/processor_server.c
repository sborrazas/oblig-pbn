#include <getopt.h>
#include "../utils/socket.h"
#include "../utils/debug.h"
#include "../utils/term.h"
#include "queue_process.h"

#define PROCESSOR_SERVER_PORT 4000

static struct option options[] = {
  {"proj_id", required_argument,  0, 'o'}
};

char* shortopts = "o:";

int main(int argc, char* const argv[]) {
    int proj_id;
    int listen_fd;
    int connfd;
    short int is_active;
    int pid;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }

    is_active = 1;
    listen_fd = socket_listen(PROCESSOR_SERVER_PORT);

    while (is_active) {
        if ((connfd = socket_accept(listen_fd)) == -1) {
            is_active = 0;
            log_warn("Error al aceptar una nueva conexión.");
        }

        pid = fork_controller("processor_controller", proj_id, connfd);

        // TODO: Handle pid
    }

    return 0;
}
