#include <getopt.h>
#include "utils/log.h"
#include "utils/socket.h"
#include "utils/debug.h"
#include "utils/term.h"
#include "utils/sig.h"
#include "modules/mq_proto.h"

#define DEFAULT_INTERVAL 10
#define DEFAULT_NUM_MESSAGES 10

static struct option origin_options[] = {
  {"address", required_argument,  0, 'a'},
  {"port",  required_argument,  0, 'p'},
  {"name",  required_argument,  0, 'n'},
  {"interval", required_argument, 0, 'i'},
  {"num_messages", required_argument, 0, 'm'}
};

void handle_exit();

char* origin_shortopts = "a:p:n:i:m:";
int conn_fd;

int main(int argc, char* const argv[]) {
    char* address;
    char* name;
    int port;
    int interval;
    int num_messages;
    Ack_Msg ack_msg;
    int i;

    if ((address = term_str_option(argc, argv, origin_options, origin_shortopts, 0)) == NULL) {
        print_err("Opción `--address` no presente. Finalizando..");
    }
    if ((port = term_int_option(argc, argv, origin_options, origin_shortopts, 1)) == -1) {
        print_err("Opción `--port` no presente. Finalizando..");
    }
    if ((name = term_str_option(argc, argv, origin_options, origin_shortopts, 2)) == NULL) {
        print_err("Opción `--name` no presente. Finalizando..");
    }
    if ((interval = term_int_option(argc, argv, origin_options, origin_shortopts, 3)) == -1) {
        print_warn("Opción `--interval` no presente, utilizando el default de %d.", DEFAULT_INTERVAL);
        interval = DEFAULT_INTERVAL;
    }
    if ((num_messages = term_int_option(argc, argv, origin_options, origin_shortopts, 4)) == -1) {
        print_warn("Opción `--num_messages` no presente, utilizando el default de %d.", DEFAULT_NUM_MESSAGES);
        num_messages = DEFAULT_NUM_MESSAGES;
    }

    log_open_stdout();

    // Llenar nombre con espacios hasta NAME_SIZE
    for (i = strlen(name); i < NAME_SIZE; i++) {
        name[i] = ' ';
    }
    name[NAME_SIZE] = '\0';

    if ((conn_fd = socket_connect(address, port)) < 0) {
        print_err("El servidor no está disponible");
    }

    if (signals_termination(NULL, handle_exit) != 0) {
        print_err("No se pudo registrar señales correctamente en message_queue.");
    }

    // Iniciar conexión
    // Mandar SYN
    print_info("Orig `%s`: Enviando SYN", name);
    mq_send_syn(conn_fd, name);
    // Recibir ACK
    print_info("Orig `%s`: Recibiendo ACK", name);
    mq_receive_ack(conn_fd, &ack_msg);

    for (i = 1; i <= num_messages; i++) {
        print_info("Orig `%s`: Enviando MSG %d", name, i);
        // Mandar MSG
        mq_send_msg(conn_fd, name, i, i % 2);
        // Recibir ACK
        mq_receive_ack(conn_fd, &ack_msg);
        sleep(interval);
    }

    // Finalizar conexión
    // Mandar FIN
    print_info("Orig `%s`: Enviando FIN", name);
    mq_send_fin(conn_fd, name);
    // Recibir ACK
    mq_receive_ack(conn_fd, &ack_msg);

    return 0;
}

void handle_exit() {
    if (conn_fd > 0) {
        socket_close(conn_fd);
    }
}
