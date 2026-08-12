#include "request_handler.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>

int request_handler(int sockfd, const char* recv_buf){
    printf("Request handler\n");
    if(get_request(recv_buf)){
        printf("GET request\n");

        // send HTTP response
        const char buf_content[] = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html\r\n"
                                  "\r\n"
                                  "<html><h1>Test</h1></html>";
        
        printf("Content buffer size: %d\n", (int)sizeof(buf_content));
        int sent_bytes = send(sockfd, buf_content, sizeof(buf_content), 0);
        if (sent_bytes == -1){
            perror("Error sending content\n");
            return -1;
        }
    }
    return 0;
}

bool get_request(const char* recv_buf){
    if (strncmp(recv_buf, "GET", 3) == 0){
        return true;
    }
    return false;
}

bool post_request(const char* recv_buf){
    if (strncmp(recv_buf, "POST", 3) == 0){
        return true;
    }
    return false;
}