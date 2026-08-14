#ifndef HANDLER_H
#define HANDLER_H

#include <stdbool.h>
#include <stddef.h>

int request_handler(int sockfd, const char* recv_buf, size_t recv_buf_size);

typedef struct{
    char* method;
    char* path;
} Request;
Request request_parse(const char* recv_buf, size_t recv_buf_size);
#endif // HANDLER_H