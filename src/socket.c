#include "include.h"

int socket_creation(uint16_t port) {

    int sock;
    struct sockaddr_in name;

    sock = socket(PF_INET, SOCK_STREAM, 0);

    if(sock < 0) {
        perror("Error: Socket failure.");
        exit(EXIT_FAILURE);
    }

    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sock,(struct sockaddr *) &name, sizeof(name)) < 0) {
        perror("Error: Failed to bind local address.");
        exit(EXIT_FAILURE);
    }

    return sock;
}

void accept_new_connection(int sock, fd_set *active_fd_set) {
    struct sockaddr_in client;
    socklen_t size = sizeof(client);
    int new_socket = accept(sock, (struct sockaddr*)&client, &size);

    if(new_socket < 0) {
        perror("Warning: Failed to accept socket connection.");
        return;
    }

    fprintf(stderr, "Connected client %s, port %hu.\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    FD_SET(new_socket, active_fd_set);
}

void close_idle_connections(fd_set *active_fd_set, int sock) {
    for(int i = 0; i < INC_FD_SETSIZE; i++) {
        if(FD_ISSET(i, active_fd_set) && i != sock) {
            closesocket(i);
            FD_CLR(i, active_fd_set);
        }
    }
}
