#include "request_handler.h"
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>


#define READ_BUFFER 4096
#define ROOT_FOLDER "www"

int request_handler(int sockfd, const char* recv_buf){
    printf("Request handler\n");

    Request req = request_parse(recv_buf);
    if (req.method[0] == '\0'){
        printf("Failed to get method\n");
        return -1;
    }

    if(strcmp("GET",req.method) == 0){
        printf("GET request\n");

        printf("method: %s, path: %s, params: %s\n", req.method, req.path, req.params);

        if (strcmp("/", req.path) == 0 || strcmp("/index.html", req.path) == 0){
            printf("index.html was requested\n");
        
            // construct index file path
            size_t index_file_path_size = strlen(ROOT_FOLDER) + strlen("index.html") + 2;
            char index_file_path[index_file_path_size];
            snprintf(index_file_path, index_file_path_size, "%s/index.html", ROOT_FOLDER);
            
            // getting file size
            struct stat st;
            memset(&st, 0, sizeof(st));

            if (stat(index_file_path, &st) == -1){
                perror("Error getting file size\n");

                // HTTP not found
                send_404_response(sockfd);
                return -1;
            }
            size_t index_file_size = st.st_size;


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

            FILE* f = fopen(index_file_path,"r");
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

    }else {
        // if not the root was requested
        printf("file requested: %s\n", req.path);
        // construct file path
        size_t file_path_size = strlen(ROOT_FOLDER) + strlen(req.path) + 2;
        char file_path[file_path_size];
        snprintf(file_path, file_path_size, "%s%s", ROOT_FOLDER, req.path);

        // getting file size
        struct stat st;
        memset(&st, 0, sizeof(st));

        if (stat(file_path, &st) == -1){
            perror("Error getting file size\n");

            // HTTP not found
            send_404_response(sockfd);
            return -1;
        }
        size_t index_file_size = st.st_size;


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
        FILE* f = fopen(file_path,"r");
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

    }

    } else if (strcmp("POST", req.method) == 0){
        printf("POST request\n");


    } else {
        // other HTTP method not allowed
        send_405_response(sockfd);
        return -1;
    }
    return 0;
}

Request request_parse(const char* recv_buf){
  Request req;
  memset(&req, 0, sizeof(req));

  for (int i = 0; i < (int)strlen(recv_buf);i++){
    if (i >= (int)sizeof(req.method)-1) return req;
    if (recv_buf[i] == ' '){

        // parsing method
        for (int y = 0; y < i; y++){
            req.method[y] = recv_buf[y];
        }
        req.method[i] = '\0';

        // parsing path
        if (recv_buf[i+1] == '/') {
            int start = i+1;
            int end = start;

            int s = 0;
            while (recv_buf[end] != '\0'){
              if (s >= (int)sizeof(req.path)-1) return req;
              if (recv_buf[end] == ' ' || recv_buf[end] == '?') break;
              req.path[s] = recv_buf[end];
              s++;
              end++;
            }
            req.path[s] = '\0';

            // parsing params
            if (recv_buf[end] == '?'){
                int start = end+1;
                int end = start;
    
                int s = 0;
                while (recv_buf[end] != '\0'){
                  if (s >= (int)sizeof(req.params)-1) return req;
                  req.params[s] = recv_buf[end];
                  s++;
                  end++;
                }
                req.params[s] = '\0';
    
            }
        }

        return req;
    }
  }
  return req;
}


// HTTP not found
int send_404_response(int sockfd) {
    const char* header_buf = "HTTP/1.1 404 Not FOund\r\n"
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
    return -1;
}

// HTTP method not allowed
int send_405_response(int sockfd) {
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
    return -1;
}
