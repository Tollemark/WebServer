#include "include.h"

void read_config_file(const char *filename, Config *config) {
    FILE *file = fopen(filename, "r");
    if(!file) {
        perror("Error: Failed to open config file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_LINE];
    while(fgets(line, sizeof(line), file)) {
        char key[MAX_LINE], value[MAX_LINE];

        if(line[0] == '#' || line[0] == '\n') continue;

        if(sscanf(line, "%[^=]=%s", key, value) == 2) {
            printf("%s: %s\n", key, value);
            if(strcmp(key, "port") == 0) {
                config->port = atoi(value);
            } else if(strcmp(key, "root_directory") == 0) {
                strncpy(config->root_directory, value, MAX_VALUE - 1);
                config->root_directory[MAX_VALUE - 1] = '\0';
            } else if(strcmp(key, "error_directory") == 0) {
                strncpy(config->error_directory, value, MAX_VALUE - 1);
                config->error_directory[MAX_VALUE - 1] = '\0';
            }
        }
    }
    fclose(file);
}

int main(int argc, char **argv) {
    int sock, nbytes;
    char buffer[MAXMSG];
    fd_set active_fd_set, read_fd_set;
    struct timeval timeout = {TIMEOUT_SECONDS, 0};

    Config config = {0};
    read_config_file("../config.ini", &config);

    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error: WSAStartup failed");
        exit(EXIT_FAILURE);
    }

    sock = socket_creation(config.port);
    if(listen(sock, 1) < 0) {
        perror("Error: Failed to listen to socket.");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fd_set);
    FD_SET(sock, &active_fd_set);

    while(1) {
        read_fd_set = active_fd_set;
        int select_result = select(INC_FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout);

        if(select_result == 0) {
            close_idle_connections(&active_fd_set, sock);
        } else if(select_result < 0) {
            printf("Warning: Select failed: %d\n", WSAGetLastError());
            continue;
        }

        for(int i = 0; i < INC_FD_SETSIZE; i++) {
            if(FD_ISSET(i, &read_fd_set)) {
                if(i == sock) {
                    accept_new_connection(sock, &active_fd_set);
                } else {
                    memset(buffer, 0, sizeof(buffer));
                    nbytes = recv(i, buffer, MAXMSG, 0);
                    if(nbytes <= 0) {
                        closesocket(i);
                        FD_CLR(i, &active_fd_set);
                    } else {
                        handle_client_request(i, config, buffer);
                    }
                }
            }
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
