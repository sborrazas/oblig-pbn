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
short int proc_receive_msg(int conn_fd, const char* name);
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

    printf("Iniciando processor_controller con proj_id = %d, conn_fd = %d\n", proj_id, conn_fd);

    if (proc_connect(conn_fd, name)) {
        while (proc_receive_msg(conn_fd, name) != -1) ;
    }

    return 0;
}

short int proc_connect(int conn_fd, char* name) {
    Syn_Msg syn_msg;

    printf("recibir syn\n");
    mq_receive_syn(conn_fd, &syn_msg);
    printf("syn recibido\n");

    printf("agregar proc\n");
    if (queue_mem_add_processor(queue_mem, semid) == -1) { // Too many procssors
        mq_send_err(conn_fd, MQ_ERR_TOO_MANY_CLIENTS, name, syn_msg.datetime);

        printf("proc no agregado\n");
        return 0;
    }
    else {
        printf("proc agregado\n");
        strcpy(name, syn_msg.name);
        mq_send_ack(conn_fd, syn_msg.name, syn_msg.datetime);

        return 1;
    }
}

short int proc_receive_msg(int conn_fd, const char* name) {
    Ack_Msg ack_msg;
    Message msg;

    printf("recibir rdy\n");
    if (!mq_receive_rdy(conn_fd)) { // FIN
        return -1;
    }
    printf("rdy recibido\n");

    printf("sacar msg\n");
    queue_mem_remove_msg(queue_mem, semid, &msg);
    printf("msg sacado\n");

    printf("enviar msg\n");
    mq_send_msg(conn_fd, name, msg.counter, msg.high_priority);
    printf("msg enviado\n");

    mq_receive_ack(conn_fd, &ack_msg);

    return 0;
}

void handle_exit() {
    log_info("Terminando processor_controller con pid = %d", getpid());
    socket_close(conn_fd);
    queue_mem_disconnect(queue_mem);
}
