## Simple webserver in C
A working webserver that is capable of handling HTTP requests, serving HTML files, 
and returning HTTP status codes.

## Build
```bash
gcc -Wall -Wextra -Wunused -o webserver webserver.c request_handler.c
```