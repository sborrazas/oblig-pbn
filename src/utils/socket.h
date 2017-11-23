

int socket_create(int port);
int socket_accept(int listenfd);
void socket_close(int connfd);

int socket_send(int connfd, char* buffer, int size);
int socket_recv(int connfd, char* buffer, int size);
