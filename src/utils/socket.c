#include "socket.h"

int socket_listen(uint16_t port) { // TODO: Manejar errores
    int listen_fd;
    struct sockaddr_in serv_addr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listen_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(listen_fd, MAX_WAITING_CONNECTIONS);

    return listen_fd;
}

int socket_accept(int listenfd) {
    return accept(listenfd, (struct sockaddr*)NULL, NULL);
}

int socket_connect(const char* address, uint16_t port) {
    int conn_fd;
    struct sockaddr_in servername;

    if ((conn_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        log_warn("No se pudo crear el socket a %s:%d", address, port);
        return -1;
    }

    servername.sin_family = AF_INET;
    servername.sin_port = htons(port);

    if (inet_aton(address, &servername.sin_addr) == 0) {
        close(conn_fd);
        log_warn("Address %s enviada es inválida", address);
        return -1;
    }

    if (connect(conn_fd, (struct sockaddr*) &servername, sizeof(servername)) == -1) {
        close(conn_fd);
        log_warn("No se pudo conectar con %s:%d", address, port);
        return -1;
    }

    return conn_fd;
}

void socket_close(int conn_fd) {
    close(conn_fd);
}

int socket_send(int conn_fd, char* buffer, int size) {
    // size = bytes faltantes por enviar
    // bytes_read = bytes enviados
    // buffer = posición en el array a partir de donde leo
    ssize_t bytes_written;

    while (size > 0) {
        if ((bytes_written = write(conn_fd, buffer, size)) == -1) {
            log_warn("Error en el socket_send al enviar %d bytes", size);
            return -1;
        }
        if (bytes_written == 0) {
            log_info("Socket finalizado");
            return -1;
        }

        size -= bytes_written;
        buffer = &buffer[bytes_written];
    }

    return 0;
}

int socket_recv(int conn_fd, char* buffer, int size) {
    // size = bytes faltantes por recibir
    // bytes_read = bytes recibidos
    // buffer = posición en el array a partir de donde escribo
    ssize_t bytes_read;

    while (size > 0) {
        if ((bytes_read = read(conn_fd, buffer, size)) == -1) {
            log_warn("Error en el socket_recv al recibir %d bytes", size);
            return -1;
        }
        if (bytes_read == 0) {
            log_info("Socket finalizado");
            return -1;
        }

        size -= bytes_read;
        buffer = &buffer[bytes_read];
    }

    return 0;
}
