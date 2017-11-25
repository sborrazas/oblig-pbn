#include "term.h"

int term_int_option(int argc, char* const argv[], const struct option* options, const char* shortopts, int index) {
    int c;
    int option_index;

    optind = 0;

    while ((c = getopt_long(argc, argv, shortopts, options, &option_index)) != -1) {
        if (option_index == index) {
            return atoi(optarg);
        }
    }

    return -1;
}

char* term_str_option(int argc, char* const argv[], const struct option* options, const char* shortopts, int index) {
    int c;
    int option_index;

    optind = 0;

    while ((c = getopt_long(argc, argv, shortopts, options, &option_index)) != -1) {
        if (option_index == index) {
            return optarg;
        }
    }

    return NULL;
}
