#include "mq_proto.h"

int mq_receive_syn(int conn_fd, Syn_Msg* syn_msg) {
    return RET_SUCCESS;
}

int mq_send_err(int conn_fd, Err_Msg err_msg) {
    return RET_SUCCESS;
}

int mq_send_ack(int conn_fd, Ack_Msg ack_msg) {
    return RET_SUCCESS;
}
