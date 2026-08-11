#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>

#define PORT 8080
#define LISTEN "127.0.0.1"
#define CONNECTIONS 100

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1){
        perror("Error occured when creating socket!\n");
        exit(-1);
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    inet_pton(AF_INET, LISTEN, &addr);
    addr.sin_port = htons(PORT);

    // bind the socketaddr
    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1){
        perror("Error binding socket\n");
    }

    if (listen(sockfd, CONNECTIONS) == -1){
        perror("Error\n");
    }


    while (1){
        
    }
    return 0;
}