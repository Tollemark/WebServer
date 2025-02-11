#include <stdbool.h>
#include "include.h"

typedef struct {
    const char *ext;
    const char *mime;
} MimeMapping;

const char* get_http_status_message(HttpStatus http_code) {
    switch(http_code) {
        case HTTP_OK: return "OK";
        case HTTP_BAD_REQUEST: return "Bad Request";
        case HTTP_FORBIDDEN: return "Forbidden";
        case HTTP_NOT_FOUND: return "Not Found";
        case HTTP_INTERNAL_ERROR: return "Internal Server Error";
        case HTTP_NOT_IMPLEMENTED: return "Not Implemented";
        default: return "Unknown Status";
    }
}

const char* get_content_type(const char *full_path) {
    static const MimeMapping mime_types[] = {
        {".html", "text/html"}, {".htm", "text/html"},
        {".css", "text/css"},
        {".js", "text/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"}, {".jpeg", "image/jpeg"},
        {".gif", "image/gif"},
        {".svg", "image/svg+xml"},
        {".ico", "image/x-icon"},
        {".json", "application/json"},
        {".xml", "application/xml"},
        {".pdf", "application/pdf"},
        {".zip", "application/zip"},
        {".mp3", "audio/mpeg"},
        {".mp4", "video/mp4"},
        {".webm", "video/webm"},
        {".ogg", "audio/ogg"},
        {".wav", "audio/wav"},
        {".aac", "audio/aac"},
        {".ttf", "font/ttf"},
        {".woff", "font/woff"},
        {".woff2", "font/woff2"},
        {".csv", "text/csv"},
        {".bin", "application/octet-stream"}
    };

    const char *ext = strrchr(full_path, '.');
    if(!ext) return "text/html";

    for(int i = 0; i < sizeof(mime_types) / sizeof(mime_types[0]); i++) {
        if(strcmp(ext, mime_types[i].ext) == 0) {
            return mime_types[i].mime;
        }
    }
    return "text/plain";
}

/* Check for traversal attacks */
bool is_endpoint_safe(char *path) {
    int length = strlen(path);
    for(int i = 0; i < length; i++) {
        if (path[i] == '+'
        || (path[i] == '.' && i + 1 < length && path[i + 1] == '.')
        || (path[i] == '%' && i + 2 < length && isxdigit(path[i + 1]) && isxdigit(path[i + 2]))) {
           return false;
        }
    }
    return true;
}

void handle_client_request(int client_socket, Config config, char *buffer) {
    char endpoint[150] = {0};

    char *token = strchr(buffer, '/');
    if(!token) return;

    int index = token - buffer;
    int counter = 0;

    while(buffer[++index] != ' ') {
        endpoint[counter++] = buffer[index];
    }

    char full_path[512];
    if(endpoint[0] == 0) {
        strcpy(endpoint, "index.html");
    } else if (!is_endpoint_safe(endpoint)) {
        snprintf(full_path, sizeof(full_path), "%s/403.html", config.error_directory);
        send_to_browser(HTTP_FORBIDDEN, client_socket, full_path);
        return;
    }

    snprintf(full_path, sizeof(full_path), "%s/%s", config.root_directory, endpoint);
    if(access(full_path, F_OK) != -1) {   
        send_to_browser(HTTP_OK, client_socket, full_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s/404.html", config.error_directory);
        send_to_browser(HTTP_NOT_FOUND, client_socket, full_path);
    }
}

void send_to_browser(HttpStatus http_code, int socket, char *full_path) {
    FILE *file = fopen(full_path, "rb");
    if(!file) {
        char header[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(socket, header, strlen(header), 0);
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *file_data = malloc(file_size);
    if(!file_data) {
        fclose(file);
        return;
    }
    fread(file_data, 1, file_size, file);
    fclose(file);

    char header[256];
    sprintf(header, "HTTP/1.1 %i %s\r\n"
                    "Content-Type: %s\r\n"
                    "Content-Length: %ld\r\n"
                    "Connection: keep-alive\r\n"
                    "Keep-Alive: timeout=5, max=100\r\n\r\n", http_code, get_http_status_message(http_code), get_content_type(full_path), file_size);
   
    send(socket, header, strlen(header), 0);

    int total_sent = 0;
    while(total_sent < file_size) {
        int bytes_sent = send(socket, file_data + total_sent, file_size - total_sent, 0);
        if(bytes_sent < 0) {
            printf("Warning: Failed to send file: %s, error: %d\n", full_path, WSAGetLastError());
            break;
        }
        total_sent += bytes_sent;
    }

    free(file_data);
}
