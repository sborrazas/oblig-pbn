#include "log.h"

FILE* log_fp = NULL;

short int log_open(char* filepath) {
    log_fp = fopen(filepath, "a");

    if (log_fp == NULL) {
        print_warn("Archivo de log %s no pudo ser abierto", filepath);
        return -1;
    }

    setbuf(log_fp, NULL);

    return 1;
}

void log_open_stdout() {
    log_fp = stdout;
}

FILE* log_current() {
    return log_fp;
}

void log_close() {
    fclose(log_fp);
}
