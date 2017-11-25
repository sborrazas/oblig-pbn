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

int term_get_menu_option(char* const menu_options[], int size) {
    short int invalid_option;
    int option;
    int i;
    int result;

    invalid_option = 1;

    printf("============== MENU ==============\n");

    while (invalid_option) {
        printf("Elija una opción (1-%d):\n", size);

        for (i = 0; i < size; i++) {
            printf("  %d. %s\n", i + 1, menu_options[i]);
        }

        printf("> ");

        if ((result = scanf("%d", &option)) == EOF) {
            invalid_option = 0;
            option = 0;
        }
        else if (result == 0) {
            while (fgetc(stdin) != '\n');
            printf("Opción inválida.\n");
        }
        else if (option > size || option < 1) {
            printf("Opción inválida.\n");
        }
        else {
            invalid_option = 0;
        }
    }

    return option;
}
