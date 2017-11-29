#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include "debug.h"

short int log_open(char* filepath);

FILE* log_current();

void log_open_stdout();

void log_close();

#define log_err(M, ...) fprintf(log_current(), "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__); exit(EXIT_FAILURE)

#define log_warn(M, ...) fprintf(log_current(), "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(log_current(), "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif
