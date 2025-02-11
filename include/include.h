#ifndef INCLUDE_H
#define INCLUDE_H
#include <ctype.h>
#include <direct.h>
#include <errno.h>
#include <io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define INC_FD_SETSIZE 1024
#define MAXMSG 5000
#define MAX_LINE 256
#define MAX_VALUE 100
#define TIMEOUT_SECONDS 5

typedef enum {
    HTTP_OK = 200,
    HTTP_BAD_REQUEST = 400,
    HTTP_FORBIDDEN = 403,
    HTTP_NOT_FOUND = 404,
    HTTP_INTERNAL_ERROR = 500,
    HTTP_NOT_IMPLEMENTED = 501
} HttpStatus;

typedef struct {
    uint16_t port;
    char root_directory[MAX_VALUE];
    char error_directory[MAX_VALUE];
} Config;

void handle_client_request(int client_socket, Config config, char buffer[]);
void send_to_browser(HttpStatus http_code, int socket, char full_path[]);
int socket_creation(uint16_t port);
void accept_new_connection(int sock, fd_set *active_fd_set);
void close_idle_connections(fd_set *active_fd_set, int sock);
#endif