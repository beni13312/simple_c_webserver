#include "request_handler.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>

#define READ_BUFFER 4096

int request_handler(int sockfd, const char* recv_buf){
    printf("Request handler\n");
    if(get_request(recv_buf)){
        printf("GET request\n");

        // send HTTP response
        const char header_buf[] = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html\r\n"
                                  "\r\n";
        
        printf("HTTP header content size: %d\n", (int)sizeof(header_buf));
        int header_bytes = send(sockfd, header_buf, sizeof(header_buf), 0);
        if (header_bytes == -1){
            perror("Error sending content\n");
            return -1;
        }
        // send HTTP content
        FILE* f = fopen("www/index.html","r");
        char file_buf[READ_BUFFER];

        int body_bytes = 0;
        int read_bytes = 0;
        while((read_bytes = fread(file_buf, 1, READ_BUFFER, f)) > 0){
            printf("read_bytes: %d\n", read_bytes);
            
            int body_sent_bytes = send(sockfd, file_buf, read_bytes, 0);
            printf("body_sent_bytes: %d\n", body_sent_bytes);
            if (body_sent_bytes == -1){
                perror("Error sending content\n");
                return -1;
            }
            body_bytes += body_sent_bytes;
        }
        printf("HTTP body content size: %d\n", body_bytes);
        fclose(f);
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