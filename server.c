#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
    int sock;
    struct sockaddr_in sin;
    const int y = 1;

    // Socket erzeugen
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0){
      printf("Socket: %i wurde angelegt!\n", sock);
    }

    // Initialisieren von sin mit geeigneten Werten
    int sin_len = sizeof(sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(4711);

    // Socket binden
    if(bind(sock, (struct sockaddr *) &sin, sizeof(sin)) != 0){
      printf("Der Port ist nicht frei - belegt!\n");
    } else {
      printf("Socket wurde gebunden!\n");
    }

    // Socket wartet auf eingehende Verbindungen
    listen(sock, 5);

    // Socket schließen
    unlink((const char *) &sin);
    return (EXIT_SUCCESS);
}
