#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include "debug.h"

FILE* log_open(char* filepath);

void log_close(const FILE* fp);

void log_info(const FILE* fp, const char* format, ...);

void log_warn(const FILE* fp, const char* format, ...);

void log_err(const FILE* fp, const char* format, ...);

#endif
