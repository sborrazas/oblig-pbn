#include <getopt.h>
#include "utils/socket.h"
#include "utils/debug.h"
#include "utils/term.h"

#define DEFAULT_INTERVAL 10
#define DEFAULT_NUM_MESSAGES 10

static struct option origin_options[] = {
  {"address", required_argument,  0, 'a'},
  {"port",  required_argument,  0, 'p'},
  {"interval", required_argument, 0, 'i'},
  {"num_messages", required_argument, 0, 'n'}
};

char* origin_shortopts = "a:p:";

int main(int argc, char* const argv[]) {
    char* address;
    int port;
    int conn_fd;
    int interval;
    int num_messages;

    if ((address = term_str_option(argc, argv, origin_options, origin_shortopts, 0)) == NULL) {
        log_err("Opción `--address` no presente. Finalizando..");
    }
    if ((port = term_int_option(argc, argv, origin_options, origin_shortopts, 1)) == -1) {
        log_err("Opción `--port` no presente. Finalizando..");
    }
    if ((interval = term_int_option(argc, argv, origin_options, origin_shortopts, 2)) == -1) {
        log_warn("Opción `--interval` no presente, utilizando el default de %d.", DEFAULT_INTERVAL);
        interval = DEFAULT_INTERVAL;
    }
    if ((num_messages = term_int_option(argc, argv, origin_options, origin_shortopts, 3)) == -1) {
        log_warn("Opción `--num_messages` no presente, utilizando el default de %d.", DEFAULT_NUM_MESSAGES);
        num_messages = DEFAULT_NUM_MESSAGES;
    }

    if ((conn_fd = socket_connect(address, port)) < 0) {
        log_err("El servidor no está disponible.");
    }

    // Iniciar conexión

    while (num_messages--) {
        // Enviar mensaje
        sleep(interval);
    }

    // Finalizar conexión

    return 0;
}
