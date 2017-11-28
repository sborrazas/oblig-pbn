#include <getopt.h>
#include "../utils/debug.h"
#include "../utils/term.h"
#include "../utils/sig.h"
#include "./queue_mem.h"
#include "./mq_proto.h"

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'},
  {"conn_fd",  required_argument,  0, 'c'}
};
static char* shortopts = "p:c:";

Queue_Mem* queue_mem;
int semid;
int conn_fd;

short int orig_connect(int conn_fd, char* name);
short int orig_receive_msg(int conn_fd, int* counter, const char* name);
void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    char name[NAME_SIZE + 1];
    int counter;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }
    if ((conn_fd = term_int_option(argc, argv, options, shortopts, 1)) == -1) {
        log_err("Opción `--conn_fd` no presente. Finalizando..");
    }

    if (signals_termination(NULL, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en origin_controller.");
    }

    log_info("Iniciando origin_controller con proj_id = %d, conn_fd = %d",
             proj_id, conn_fd);

    if ((queue_mem = queue_mem_connect(proj_id, &semid)) == NULL) {
        log_err("origin_controller no pudo conectarse a shared_mem");
    }

    if (orig_connect(conn_fd, name)) {
        counter = 0;

        while (orig_receive_msg(conn_fd, &counter, name) != -1) ;
    }

    return 0;
}

short int orig_connect(int conn_fd, char* name) {
    Syn_Msg syn_msg;

    mq_receive_syn(conn_fd, &syn_msg);
    log_info("SYN(%s) recibido de orig con pid = %d", syn_msg.name, getpid());

    if (queue_mem_add_origin(queue_mem, semid) == -1) { // Too many origins
        mq_send_err(conn_fd, MQ_ERR_TOO_MANY_CLIENTS, name, syn_msg.datetime);

        return 0;
    }
    else {
        strcpy(name, syn_msg.name);
        mq_send_ack(conn_fd, syn_msg.name, syn_msg.datetime);
        log_info("ACK enviado a orig con pid = %d", getpid());

        return 1;
    }
}

short int orig_receive_msg(int conn_fd, int* counter, const char* name) {
    Msg_Msg msg_msg;
    Message msg;

    if (!mq_receive_msg(conn_fd, &msg_msg)) { // FIN
        log_info("FIN recibido de orig con pid = %d", getpid());
        return -1;
    }

    log_info("MSG(%d %d) recibido de orig con pid = %d", msg_msg.counter,
             msg_msg.high_priority, getpid());

    if (msg_msg.counter > *counter) {
        strcpy(msg.orig_name, name);
        msg.counter = msg_msg.counter;
        msg.high_priority = msg_msg.high_priority;
        strcpy(msg.datetime, msg_msg.datetime);

        queue_mem_add_msg(queue_mem, semid, &msg);
        mq_send_ack(conn_fd, name, msg_msg.datetime);
        log_info("ACK(%d) enviado a orig con pid = %d", msg_msg.counter, getpid());
        *counter = msg_msg.counter;

        return 1;
    }
    else {
        log_info("ERR(%d) enviado a orig con pid = %d", msg_msg.counter, getpid());
        mq_send_err(conn_fd, MQ_ERR_INVALID_COUNTER, name, msg_msg.datetime);

        return 0;
    }
}

void handle_exit() {
    log_info("Terminando origin_controller con pid = %d", getpid());
    socket_close(conn_fd);
    queue_mem_disconnect(queue_mem);
}
