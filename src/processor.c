#include <getopt.h>
#include "utils/socket.h"
#include "utils/debug.h"
#include "utils/term.h"
#include "utils/sig.h"
#include "modules/mq_proto.h"

#define DEFAULT_INTERVAL 10
#define DEFAULT_NUM_MESSAGES 10

static struct option processor_options[] = {
  {"address", required_argument,  0, 'a'},
  {"port",  required_argument,  0, 'p'},
  {"name",  required_argument,  0, 'n'},
  {"interval", required_argument, 0, 'i'},
  {"num_messages", required_argument, 0, 'm'}
};

void handle_exit();

char* processor_shortopts = "a:p:n:i:m:";
int conn_fd;

int main(int argc, char* const argv[]) {
    char* address;
    char* name;
    int port;
    int interval;
    int num_messages;
    Ack_Msg ack_msg;
    Msg_Msg msg_msg;
    int i;

    if ((address = term_str_option(argc, argv, processor_options, processor_shortopts, 0)) == NULL) {
        log_err("Opción `--address` no presente. Finalizando..");
    }
    if ((port = term_int_option(argc, argv, processor_options, processor_shortopts, 1)) == -1) {
        log_err("Opción `--port` no presente. Finalizando..");
    }
    if ((name = term_str_option(argc, argv, processor_options, processor_shortopts, 2)) == NULL) {
        log_err("Opción `--name` no presente. Finalizando..");
    }
    if ((interval = term_int_option(argc, argv, processor_options, processor_shortopts, 3)) == -1) {
        log_warn("Opción `--interval` no presente, utilizando el default de %d.", DEFAULT_INTERVAL);
        interval = DEFAULT_INTERVAL;
    }
    if ((num_messages = term_int_option(argc, argv, processor_options, processor_shortopts, 4)) == -1) {
        log_warn("Opción `--num_messages` no presente, utilizando el default de %d.", DEFAULT_NUM_MESSAGES);
        num_messages = DEFAULT_NUM_MESSAGES;
    }

    if ((conn_fd = socket_connect(address, port)) < 0) {
        log_err("El servidor no está disponible.");
    }

    if (signals_termination(NULL, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en message_queue.");
    }

    // Iniciar conexión
    // Mandar SYN
    mq_send_syn(conn_fd, name);
    // Recibir ACK
    mq_receive_ack(conn_fd, &ack_msg);

    for (i = 1; i <= num_messages; i++) {
        // Mandar RDY
        log_info("Proc %s: Enviando RDY %d", name, i);
        mq_send_rdy(conn_fd, name);
        // Recibir ACK
        mq_receive_ack(conn_fd, &ack_msg);
        // Recibir MSG
        mq_receive_msg(conn_fd, &msg_msg);
        log_info("Proc %s: Recibiendo MSG de %s: %d", name, msg_msg.name, msg_msg.counter);
        // Envio ACK
        mq_send_ack(conn_fd, name, ack_msg.datetime);

        sleep(interval);
    }

    // Finalizar conexión
    // Mandar FIN
    log_info("Proc %s: Enviando FIN", name);
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
