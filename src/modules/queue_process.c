#include "queue_process.h"

int fork_server(const char* name, int proj_id, int port) {
    char sproj_id[9];
    char sport[9];
    int pid;

    sprintf(sproj_id, "%d", proj_id);
    sprintf(sport, "%d", port);

    if ((pid = fork()) == 0) {
        if (execl(name, name, "--proj_id", sproj_id, "--port", sport, NULL) == -1) {
            log_err("No se pudo ejecutar %s", name);
        }
    }

    return pid;
}

int fork_controller(const char* name, int proj_id, int conn_fd) {
    char sproj_id[9];
    char sconn_fd[9];
    int pid;

    sprintf(sproj_id, "%d", proj_id);
    sprintf(sconn_fd, "%d", conn_fd);

    if ((pid = fork()) == 0) {
        if (execl(name, name, "--proj_id", sproj_id, "--conn_fd", sconn_fd, NULL) == -1) {
            log_err("No se pudo ejecutar %s", name);
        }
    }

    return pid;
}
