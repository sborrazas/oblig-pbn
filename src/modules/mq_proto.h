#ifndef __MQ_PROTO_H__
#define __MQ_PROTO_H__

#include <time.h>

#define RET_SUCCESS 0
#define RET_CONN_ERR -1
#define RET_INVALID_FORMAT -2

#define NAME_SIZE 8

#define MQ_ERR_INVALID_MSG 2

typedef struct syn_msg {
    char name[NAME_SIZE];
    time_t time;
} Syn_Msg;

typedef struct err_msg {
    int err_num;
    char* name;
    time_t time;
} Err_Msg;

typedef struct ack_msg {
    char* name;
    time_t time;
} Ack_Msg;

int mq_receive_syn(int conn_fd, Syn_Msg* syn_msg);
int mq_send_err(int conn_fd, Err_Msg err_msg);
int mq_send_ack(int conn_fd, Ack_Msg ack_msg);

#endif
