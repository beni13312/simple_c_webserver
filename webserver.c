#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "request_handler.h"

#define PORT 8080
#define LISTEN "127.0.0.1"
#define CONNECTIONS 10
#define MAX_REQUEST_SIZE 4096

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("Error when creating socket\n");
        exit(-1);
    }
    
    // allow socket to reuse address
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, LISTEN, &addr.sin_addr);
    addr.sin_port = htons(PORT);


    // binding the socket
    if (bind(sockfd, (struct sockaddr*) &addr, addr_len) == -1){
        perror("Error binding socket\n");
        exit(-1);
    }

    if (listen(sockfd, CONNECTIONS) == -1){
        perror("Error listening on socket\n");
        exit(-1);
    }


    while (1){
        struct sockaddr addr_client;
        socklen_t addr_client_len = sizeof(addr_client);

        // accepting client connections
        int sockfd_accept = accept(sockfd, (struct sockaddr*) &addr_client, &addr_client_len);

        if(sockfd_accept == -1){
            perror("Error accepting connection\n");
        }

        char recv_buf[MAX_REQUEST_SIZE];

        // reciving data from clients
        int recived_bytes = recv(sockfd_accept, recv_buf, MAX_REQUEST_SIZE, 0);

        if (recived_bytes == -1){
            perror("Error reciving data from client\n");
        }

        if (recived_bytes > 0){
            printf("Recived bytes: %i\n",recived_bytes);
            printf("Recived client request: %s\n", recv_buf);

            // handling client request
            if (request_handler(sockfd_accept, recv_buf) == -1){
                perror("Error at handling request\n");
            }
        }
        close(sockfd_accept);

        sleep(1);
    }
    close(sockfd);
    return 0;
}