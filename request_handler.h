#ifndef HANDLER_H
#define HANDLER_H

#include <stdbool.h>

int request_handler(int sockfd, const char* recv_buf);

bool get_request(const char* recv_buf);
bool post_request(const char* recv_buf);
#endif // HANDLER_H