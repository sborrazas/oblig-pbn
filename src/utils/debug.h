#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <error.h>

#define clean_errno() (errno == 0 ? "None" : strerror(errno))

#define log_err(M, ...) error_at_line(EXIT_FAILURE, errno, __FILE__, __LINE__, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_warn(M, ...) error_at_line(EXIT_SUCCESS, errno, __FILE__, __LINE__, "[WARN] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, _ _LINE__, ##__VA_ARGS__)

#define assert(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; }
#endif

