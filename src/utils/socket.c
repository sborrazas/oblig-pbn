#include "socket.h"

int socket_create(int port) {
    int listenfd;
    struct sockaddr_in serv_addr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listenfd, MAX_WAITING_CONNECTIONS);

    return listenfd;
}

int socket_accept(int listenfd) {
    return accept(listenfd, (struct sockaddr*)NULL, NULL);
}

void socket_close(int connfd) {
    close(connfd);
}

int socket_send(int connfd, char* buffer, int size) {
    // size = bytes faltantes por enviar
    // bytes_read = bytes enviados
    // buffer = posición en el array a partir de donde leo
    ssize_t bytes_written;

    while (size > 0) {
        if ((bytes_written = write(connfd, buffer, size)) == -1) {
            log_warn("Error en el socket_send al enviar %d bytes.", size);
            return -1;
        }

        size -= bytes_written;
        buffer = &buffer[bytes_written];
    }

    return 0;
}

int socket_recv(int connfd, char* buffer, int size) {
    // size = bytes faltantes por recibir
    // bytes_read = bytes recibidos
    // buffer = posición en el array a partir de donde escribo
    ssize_t bytes_read;

    while (size > 0) {
        if ((bytes_read = read(connfd, buffer, size)) == -1) {
            log_warn("Error en el socket_recv al recibir %d bytes.", size);
            return -1;
        }

        size -= bytes_read;
        buffer = &buffer[bytes_read];
    }

    return 0;
}
