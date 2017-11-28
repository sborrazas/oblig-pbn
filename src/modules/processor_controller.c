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

short int proc_connect(int conn_fd, char* name);
short int proc_send_msg(int conn_fd, const char* name);
void handle_exit();

int main(int argc, char* const argv[]) {
    int proj_id;
    char name[NAME_SIZE];

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }
    if ((conn_fd = term_int_option(argc, argv, options, shortopts, 1)) == -1) {
        log_err("Opción `--conn_fd` no presente. Finalizando..");
    }

    if (signals_termination(NULL, handle_exit) != 0) {
        log_err("No se pudo registrar señales correctamente en origin_controller.");
    }

    log_info("Iniciando processor_controller con proj_id = %d, conn_fd = %d\n",
             proj_id, conn_fd);

    if ((queue_mem = queue_mem_connect(proj_id, &semid)) == NULL) {
        log_err("processor_controller no pudo conectarse a shared_mem");
    }

    if (proc_connect(conn_fd, name)) {
        while (proc_send_msg(conn_fd, name)) ;
    }

    return 0;
}

short int proc_connect(int conn_fd, char* name) {
    Syn_Msg syn_msg;

    if (!mq_receive_syn(conn_fd, &syn_msg)) {
        log_err(log_fp, "Ocurrió un error en la conexión del origen al obtener SYN.");
    }

    log_info(log_fp, "SYN(%s) recibido de proc con pid = %d", syn_msg.name, getpid());

    if (queue_mem_add_processor(queue_mem, semid) == -1) { // Too many procssors
        mq_send_err(conn_fd, MQ_ERR_TOO_MANY_CLIENTS, name, syn_msg.datetime);
        log_info("ERR enviado a proc con pid = %d (TOO_MANY_CLIENTS)", getpid());

        return 0;
    }
    else {
        strcpy(name, syn_msg.name);
        mq_send_ack(conn_fd, syn_msg.name, syn_msg.datetime);
        log_info("ACK enviado a proc con pid = %d", getpid());

        return 1;
    }
}

short int proc_send_msg(int conn_fd, const char* name) {
    Ack_Msg ack_msg;
    Message msg;
    short int result;

    if ((result = mq_receive_rdy(conn_fd)) == -1) {
        log_err(log_fp, "Ocurrió un error en la conexión del origen al obtener SYN.");
        return -1;
    }
    else if (result == 0) { // FIN
        log_info(log_fp, "FIN recibido de proc con pid = %d", getpid());
        return 0;
    }

    log_info(log_fp, "RDY recibido de proc con pid = %d", getpid());

    queue_mem_remove_msg(queue_mem, semid, &msg);

    mq_send_msg(conn_fd, name, msg.counter, msg.high_priority);
    log_info("MSG(%d %d) enviado a proc con pid = %d", msg.counter,
             msg.high_priority, getpid());

    mq_receive_ack(conn_fd, &ack_msg);
    log_info("ACK(%d) enviado a proc con pid = %d", msg.counter, getpid());

    return 1;
}

void handle_exit() {
    log_info("Terminando processor_controller con pid = %d", getpid());
    socket_close(conn_fd);
    queue_mem_disconnect(queue_mem);
}
