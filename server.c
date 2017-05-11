#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int sock;
    struct sockaddr_in sin;

    // Socket erzeugen
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0){
      printf("Socket: %i\n", sock);
    }

    // Socket binden
    bind(sock, (struct sockaddr *)& sin, sizeof(sin));

    // Socket wartet auf eingehende Verbindungen
    listen(sock, 5);

    // Socket schließen
    unlink((const char *)& sin);
    return (EXIT_SUCCESS);
}
