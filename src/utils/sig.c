#include "sig.h"

void handle_termination();

int signals_termination(void (*handle_sigchld)(int), void (*handle_exit)(void)) {
    if (atexit(handle_exit) != 0) {
        return 1;
    }

    if (signal(SIGINT, SIG_IGN) != SIG_IGN) {
        signal(SIGINT, handle_termination);
    }
    if (signal(SIGQUIT, SIG_IGN) != SIG_IGN) {
        signal(SIGQUIT, handle_termination);
    }
    if (signal(SIGTERM, SIG_IGN) != SIG_IGN) {
        signal(SIGTERM, handle_termination);
    }
    if (handle_sigchld != NULL) {
        if (signal(SIGCHLD, SIG_IGN) != SIG_IGN) {
            signal(SIGCHLD, handle_sigchld);
        }
    }

    return 0;
}

void handle_termination() {
    log_info("SIGINT lanzada a pid = %d", getpid());

    exit(EXIT_SUCCESS);
}
