#include <getopt.h>
#include "../utils/debug.h"
#include "../utils/term.h"
#include "./queue_mem.h"
#include "./mq_proto.h"

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'},
  {"conn_fd",  required_argument,  0, 'c'}
};

char* shortopts = "p:c:";

Queue_Mem* queue_mem;
int semid;

void proc_connect(int conn_fd, char* name);

int main(int argc, char* const argv[]) {
    int proj_id;
    int conn_fd;
    char name[NAME_SIZE];
    short int is_active;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 1)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }
    if ((conn_fd = term_int_option(argc, argv, options, shortopts, 1)) == -1) {
        log_err("Opción `--conn_fd` no presente. Finalizando..");
    }
    if ((queue_mem = queue_mem_connect(proj_id, &semid)) == NULL) {
        log_err("processor_controller no pudo conectarse a shared_mem");
    }

    proc_connect(conn_fd, name);
    is_active = 1;

    while (is_active) {
        // msg = mq_receive_msg(conn_fd, &counter);
        // TODO: Intercambiar datos
    }

    return 0;
}

void proc_connect(int conn_fd, char* name) {
    // TODO
}
