#include "request_handler.h"
#include <string.h>
#include <stdio.h>

int request_handler(const char* recv_buf){
    if (strncmp(recv_buf, "GET", 3)){
        printf("GET request\n");
        return 0;
    }
    return 0;
}