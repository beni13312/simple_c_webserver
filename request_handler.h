#ifndef HANDLER_H
#define HANDLER_H

int request_handler(int sockfd, const char* recv_buf);

typedef struct{
    char method[16];
    char path[256];
    char params[512];
} Request;
Request request_parse(const char* recv_buf);

int safe_path(const char* path, const char* root_path);

int send_404_response(int sockfd);
int send_405_response(int sockfd);
#endif // HANDLER_H