#include <getopt.h>
#include "../utils/debug.h"
#include "../utils/term.h"
#include "./queue_mem.h"
#include "./mq_proto.h"

static struct option options[] = {
  {"proj_id", required_argument,  0, 'p'},
  {"conn_fd",  required_argument,  0, 'c'}
};
static char* shortopts = "p:c:";

Queue_Mem* queue_mem;

int orig_connect(int conn_fd, char* name);
int orig_receive_msg(int conn_fd, char* name);

int main(int argc, char* const argv[]) {
    int proj_id;
    int conn_fd;
    char name[NAME_SIZE];
    short int is_active;

    if ((proj_id = term_int_option(argc, argv, options, shortopts, 0)) == -1) {
        log_err("Opción `--proj_id` no presente. Finalizando..");
    }
    if ((conn_fd = term_int_option(argc, argv, options, shortopts, 1)) == -1) {
        log_err("Opción `--conn_fd` no presente. Finalizando..");
    }

    printf("Iniciando origin_controller con proj_id = %d, conn_fd = %d\n", proj_id, conn_fd);

    if ((queue_mem = queue_mem_connect(proj_id)) == NULL) {
        log_err("origin_controller no pudo conectarse a shared_mem");
    }

    is_active = orig_connect(conn_fd, name);

    while (is_active) {
        is_active = orig_receive_msg(conn_fd, name) == 0;
    }

    return 0;
}

int orig_connect(int conn_fd, char* name) {
    int ret_code;
    Syn_Msg syn_msg;
    Err_Msg err_msg;
    Ack_Msg ack_msg;

    do {
        if ((ret_code = mq_receive_syn(conn_fd, &syn_msg)) == RET_CONN_ERR) {
            log_err("Ocurrió un error en la conexión del origen.");
        }
        else if (ret_code == RET_INVALID_FORMAT) {
            err_msg.err_num = MQ_ERR_INVALID_MSG;
            err_msg.name = syn_msg.name;
            err_msg.time = syn_msg.time;
            mq_send_err(conn_fd, err_msg);
        }
    } while (ret_code != RET_SUCCESS);

    ack_msg.name = syn_msg.name;
    ack_msg.time = syn_msg.time;
    mq_send_ack(conn_fd, ack_msg);

    name = syn_msg.name; // TODO: Local memory?

    if (queue_mem_add_origin(queue_mem, name) == -1) { // Too many origins
        err_msg.err_num = MQ_ERR_TOO_MANY_CLIENTS;
        err_msg.name = syn_msg.name;
        err_msg.time = syn_msg.time;
        mq_send_err(conn_fd, err_msg);

        return 0;
    }
    else {
        return 1;
    }
}

int orig_receive_msg(int conn_fd, char* name) {
    // TODO: msg = mq_receive_msg(conn_fd, &counter);
    return 0;
}
