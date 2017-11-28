#include "mq_proto.h"

#define SPC "%*[ \t]"
#define CHR "[^\n]"

void get_current_date(char* datetime);

short int mq_receive_syn(int conn_fd, Syn_Msg* syn_msg) {
    char buffer[33];
    char name[NAME_SIZE + 1];
    char datetime[DATE_SIZE + 1];
    short int is_valid;

    is_valid = 0;

    while (!is_valid) {
        if (socket_recv(conn_fd, buffer, 32) == -1) {
            return 0;
        }
        buffer[32] = '\0';

        if (sscanf(buffer, "SYN" SPC "%8"CHR SPC "%19"CHR, name, datetime) == 2) {
            strncpy(syn_msg->name, name, NAME_SIZE);
            syn_msg->name[NAME_SIZE] = '\0';
            strncpy(syn_msg->datetime, datetime, DATE_SIZE);
            syn_msg->datetime[DATE_SIZE] = '\0';
            is_valid = 1;
        }
        else {
            mq_send_err(conn_fd, MQ_ERR_INVALID_MSG, DEFAULT_NAME, DEFAULT_DATETIME);
        }
    }

    return 1;
}

short int mq_receive_rdy(int conn_fd) {
    char buffer[33];
    char name[NAME_SIZE + 1];
    char datetime[DATE_SIZE + 1];

    while (1) {
        if (socket_recv(conn_fd, buffer, 32) == -1) {
            return -1;
        }
        buffer[32] = '\0';

        if (sscanf(buffer, "RDY" SPC "%8"CHR SPC "%19"CHR, name, datetime) == 2) {
            if (mq_send_ack(conn_fd, name, datetime) == -1) {
                log_err("Ocurrió un error en la conexión del proc al enviar ACK.");
            }

            return 1;
        }
        else if (sscanf(buffer, "FIN" SPC "%8"CHR SPC "%19"CHR, name, datetime) == 2) {
            if (mq_send_ack(conn_fd, name, datetime) == -1) {
                log_err("Ocurrió un error en la conexión del proc al enviar ACK.");
            }

            return 0;
        }
        else {
            mq_send_err(conn_fd, MQ_ERR_INVALID_MSG, DEFAULT_NAME, DEFAULT_DATETIME);
        }
    }
}

short int mq_receive_msg(int conn_fd, Msg_Msg* msg_msg) {
    char buffer[39];
    char name[9];
    char datetime[20];
    int counter;
    short int is_valid;

    is_valid = 0;

    while (!is_valid) {
        if (socket_recv(conn_fd, buffer, 4) == -1) {
            return -1;
        }
        buffer[4] = '\0';

        if (strcmp("FIN ", buffer) == 0) {
            if (socket_recv(conn_fd, buffer, 28) == -1) {
                return -1;
            }

            if (sscanf(buffer, "%8"CHR SPC "%19"CHR, name, datetime) == 2) {
                if (mq_send_ack(conn_fd, name, datetime) == -1) {
                    log_err("Ocurrió un error en la conexión al enviar ACK.");

                    return -1;
                }
            }
            else { // Terminar conexión de todas formas
                mq_send_ack(conn_fd, DEFAULT_NAME, DEFAULT_DATETIME);
            }

            return 0;
        }
        else if (strcmp("MSG ", buffer) == 0) {
            if (socket_recv(conn_fd, buffer, 38) == -1) {
                return -1;
            }
            buffer[38] = '\0';

            if (sscanf(buffer, "%8"CHR SPC "%04d" SPC "ALTA" SPC "%19"CHR, name, &counter, datetime) == 3) {
                strncpy(msg_msg->name, name, NAME_SIZE);
                msg_msg->name[NAME_SIZE] = '\0';
                strncpy(msg_msg->datetime, datetime, DATE_SIZE);
                msg_msg->datetime[DATE_SIZE] = '\0';
                msg_msg->counter = counter;
                msg_msg->high_priority = 1;
                is_valid = 1;
            }
            else if (sscanf(buffer, "%8"CHR SPC "%04d" SPC "BAJA" SPC "%19"CHR, name, &counter, datetime) == 3) {
                strncpy(msg_msg->name, name, NAME_SIZE);
                msg_msg->name[NAME_SIZE] = '\0';
                strncpy(msg_msg->datetime, datetime, DATE_SIZE);
                msg_msg->datetime[DATE_SIZE] = '\0';
                msg_msg->counter = counter;
                msg_msg->high_priority = 0;
                is_valid = 1;
            }
            else {
                mq_send_err(conn_fd, MQ_ERR_INVALID_MSG, DEFAULT_NAME, DEFAULT_DATETIME);
            }
        }
        else {
            if (mq_send_err(conn_fd, MQ_ERR_INVALID_MSG, DEFAULT_NAME, DEFAULT_DATETIME) == -1) {
                return -1;
            }
        }
    }

    return 1;
}

short int mq_send_err(int conn_fd, int err_num, const char* name, const char* datetime) {
    char buffer[36];

    snprintf(buffer, 36, "ERR %02d %8s %19s", err_num, name, datetime);

    if (socket_send(conn_fd, buffer, 35) == -1) {
        return -1;
    }

    return 1;
}

short int mq_send_ack(int conn_fd, const char* name, const char* datetime) {
    char buffer[33];

    snprintf(buffer, 33, "ACK %8s %19s", name, datetime);

    if (socket_send(conn_fd, buffer, 32) == -1) {
        return -1;
    }

    return 1;
}

void mq_send_syn(int conn_fd, const char* name) {
    char datetime[20];
    char buffer[33];

    get_current_date(datetime);

    snprintf(buffer, 33, "SYN %8s %19s", name, datetime);

    if (socket_send(conn_fd, buffer, 32) == -1) {
        log_err("Ocurrió un error en la conexión del origen al enviar SYN.");
    }
}

void mq_send_rdy(int conn_fd, const char* name) {
    char datetime[20];
    char buffer[33];

    get_current_date(datetime);

    snprintf(buffer, 33, "RDY %8s %19s", name, datetime);

    if (socket_send(conn_fd, buffer, 32) == -1) {
        log_err("Ocurrió un error en la conexión del origen al enviar RDY.");
    }
}

void mq_receive_ack(int conn_fd, Ack_Msg* ack_msg) {
    char buffer[33];
    char name[NAME_SIZE + 1];
    char datetime[DATE_SIZE + 1];

    if (socket_recv(conn_fd, buffer, 32) == -1) {
        log_err("Ocurrió un error en la conexión del origen al obtener SYN.");
    }
    buffer[32] = '\0';

    if (sscanf(buffer, "ACK" SPC "%8"CHR SPC "%19"CHR, name, datetime) == 2) {
        strncpy(ack_msg->name, name, NAME_SIZE);
        ack_msg->name[NAME_SIZE] = '\0';
        strncpy(ack_msg->datetime, datetime, DATE_SIZE);
        ack_msg->datetime[DATE_SIZE] = '\0';
    }
}

void mq_send_msg(int conn_fd, const char* name, int counter, short int high_priority) {
    char datetime[20];
    char buffer[43];

    get_current_date(datetime);

    if (high_priority) {
        snprintf(buffer, 43, "MSG %8s %04d ALTA %19s", name, counter, datetime);
    }
    else {
        snprintf(buffer, 43, "MSG %8s %04d BAJA %19s", name, counter, datetime);
    }

    if (socket_send(conn_fd, buffer, 42) == -1) {
        log_err("Ocurrió un error en la conexión del origen al enviar SYN.");
    }
}

void mq_send_fin(int conn_fd, const char* name) {
    char datetime[20];
    char buffer[33];

    get_current_date(datetime);

    snprintf(buffer, 33, "FIN %8s %19s", name, datetime);

    if (socket_send(conn_fd, buffer, 32) == -1) {
        log_err("Ocurrió un error en la conexión del origen al enviar FIN.");
    }
}

void get_current_date(char* datetime) {
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(datetime, 80, "%Y-%M-%d %H:%m:%S", timeinfo);
}
