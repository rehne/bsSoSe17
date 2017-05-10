#include <sys/socket.h>
#include <stdio.h>

int create_socket;
int main() {
    if ((create_socket = socket(AF_INET, SOCK_STREAM, 0) > 0)) {
        printf("Socket wurde angelegt\n");
    } else {
        printf("Socket konnte nicht angelegt werden.\n");
    }
}