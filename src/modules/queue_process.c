#include "queue_process.h"

int fork_server(const char* servername, int proj_id) {
    char sproj_id[9];
    int pid;

    sprintf(sproj_id, "%d", proj_id);

    if ((pid = fork()) == 0) {
        execl(servername, sproj_id);
    }

    return pid;
}

int fork_controller(const char* controllername, int proj_id, int socket_fd) {
    char sproj_id[9];
    char ssocket_fd[9];
    int pid;

    sprintf(sproj_id, "%d", proj_id);
    sprintf(ssocket_fd, "%d", socket_fd);

    if ((pid = fork()) == 0) {
        execl(controllername, sproj_id, ssocket_fd);
    }

    return pid;
}
