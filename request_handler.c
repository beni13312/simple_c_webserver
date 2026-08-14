#include "request_handler.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/stat.h>


#define READ_BUFFER 4096
#define INDEX_FILE "www/index.html"

int request_handler(int sockfd, const char* recv_buf, size_t recv_buf_size){
    printf("Request handler\n");
    if(strcmp("GET", request_parse(recv_buf, recv_buf_size).method) == 0){
        printf("GET request\n");

        // getting file size
        struct stat st;
        if (stat(INDEX_FILE, &st) == -1){
            perror("Error getting file size\n");
            return -1;
        }
        long index_file_size = st.st_size;


        char header_buf[1024];
        // send HTTP response
        const int header_len = snprintf(header_buf, sizeof(header_buf), "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/html charset=UTF-8\r\n"
                                  "Content-Length: %ld\r\n"
                                  "Connection: close\r\n"
                                  "\r\n",
                                index_file_size);
        
        printf("HTTP header content size: %d\n", header_len);
        int header_bytes = send(sockfd, header_buf, header_len, 0);
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
        fclose(f);
        printf("HTTP body content size: %d\n", body_bytes);

    } else if (strcmp("POST", request_parse(recv_buf, recv_buf_size).method) == 0){
        printf("POST request\n");


    } else {
        // other HTTP method not allowed
          const char* header_buf = "HTTP/1.1 405 Method Not Allowed\r\n"
                                  "Content-Length: 0\r\n"
                                  "Allow: GET, POST\r\n"
                                  "Connection: close\r\n"
                                  "\r\n";
        
        printf("HTTP header content size: %d\n", (int)strlen(header_buf));
        int header_bytes = send(sockfd, header_buf, strlen(header_buf), 0);
        if (header_bytes == -1){
            perror("Error sending content\n");
            return -1;
        }
    }
    return 0;
}

Request request_parse(const char* recv_buf, size_t recv_buf_size){
    char local_buf[recv_buf_size];
    memcpy(local_buf, recv_buf, strlen(recv_buf));

    Request req;
    memset(&req, 0, sizeof(req));

    req.method  = strtok(local_buf, " ");
    req.path = strtok(local_buf, " ");
    return req;
}