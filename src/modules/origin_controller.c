#include <getopt.h>
#include "../utils/debug.h"
#include "../utils/term.h"

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'},
  {"conn_fd",  required_argument,  0, 'c'}
};

char* shortopts = "p:c:";

void orig_connect(int conn_fd, char* name);

int main(int argc, char* const argv[]) {
    int proj_id;
    int conn_fd;
    short int is_active;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }
    if ((conn_fd = term_int_option(argc, argv, options, shortopts, 1)) == -1) {
        log_err("Opción `--conn_fd` no presente. Finalizando..");
    }

    is_active = 1;

    while (is_active) {
        // TODO: Intercambiar datos
    }

    return 0;
}
