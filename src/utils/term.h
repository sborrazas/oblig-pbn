#ifndef __TERM_H__
#define __TERM_H__

#include <getopt.h>
#include "debug.h"

int term_int_option(int argc, char* const argv[], const struct option* options, const char* shortopts, int index);
char* term_str_option(int argc, char* const argv[], const struct option* options, const char* shortopts, int index);

int term_get_menu_option(char* const menu_options[], int size);

#endif
