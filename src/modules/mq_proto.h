#ifndef __MQ_PROTO_H__
#define __MQ_PROTO_H__

#include <time.h>
#include "../utils/socket.h"

#define NAME_SIZE 8
#define DATE_SIZE 19

#define MQ_ERR_TOO_MANY_CLIENTS 1
#define MQ_ERR_INVALID_MSG      2
#define MQ_ERR_INVALID_COUNTER  3

#define DEFAULT_NAME "-UNINIT-"
#define DEFAULT_DATETIME "0000-00-00 00:00:00"

typedef struct syn_msg {
    char name[NAME_SIZE + 1];
    char datetime[DATE_SIZE + 1];
} Syn_Msg;

/* typedef struct err_msg { */
/*     int err_num; */
/*     char name[NAME_SIZE]; */
/*     char datetime[DATE_SIZE]; */
/* } Err_Msg; */

/* typedef struct ack_msg { */
/*     char name[NAME_SIZE]; */
/*     char datetime[DATE_SIZE]; */
/* } Ack_Msg; */

typedef struct Orig_msg {
    char name[NAME_SIZE + 1];
    int counter;
    short int priority;
    char datetime[DATE_SIZE + 1];
} Orig_Msg;

void mq_receive_syn(int conn_fd, Syn_Msg* syn_msg);
short int mq_receive_orig_msg(int conn_fd, Orig_Msg* orig_msg);
void mq_send_err(int conn_fd, int err_num, const char* name, const char* datetime);
void mq_send_orig_ack(int conn_fd, const char* name, const char* datetime);

#endif
