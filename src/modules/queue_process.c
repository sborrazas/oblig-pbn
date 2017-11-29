#include "queue_process.h"


int fork_server(const char* name, const char* logpath, int proj_id) {
    char sproj_id[9];
    int pid;

    sprintf(sproj_id, "%d", proj_id);

    if ((pid = fork()) == 0) {
        if (execl(name, name,
                  "--log_file", logpath,
                  "--proj_id", sproj_id, NULL) == -1) {
            log_err("No se pudo ejecutar %s", name);
        }
    }

    return pid;
}

int fork_controller(const char* name, const char* logpath, int proj_id, int conn_fd) {
    char sproj_id[9];
    char sconn_fd[9];
    int pid;

    sprintf(sproj_id, "%d", proj_id);
    sprintf(sconn_fd, "%d", conn_fd);

    if ((pid = fork()) == 0) {
        if (execl(name, name,
                  "--log_file", logpath,
                  "--proj_id", sproj_id,
                  "--conn_fd", sconn_fd, NULL) == -1) {
            log_err("No se pudo ejecutar %s", name);
        }
    }

    return pid;
}
