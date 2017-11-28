#include "log.h"

FILE* log_open(char* filepath) {
    FILE* fp;

    fp = fopen(filepath, "a");

    if (fp == NULL) {
        print_warn("Archivo de log %s no pudo ser abierto", filepath);
        return NULL;
    }

    return fp;
}

void log_close(FILE* fp) {
    fclose(fp);
}

void log_write(const FILE* fp, const char* format, ...) {
    fprintf(fp, format, ##__VA_ARGS__ "\n");
}

void log_info(const FILE* fp, const char* format, ...) {
    log_write(fp, format, ##__VA_ARGS__);
}

void log_warn(const FILE* fp, const char* format, ...) {
    log_write(fp, format, ##__VA_ARGS__);
}

void log_err(const FILE* fp, const char* format, ...) {
    log_write(fp, format, ##__VA_ARGS__);
    exit(EXIT_FAILURE);
}
