#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF 1024

int main(){
    int sock, new_sock;
    socklen_t addrlen;
    ssize_t size;
    struct sockaddr_in sin;
    char *buffer = malloc(BUF);

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

    addrlen = sizeof(struct sockaddr_in);

    while(1){
      new_sock = accept(sock, (struct sockaddr *) &sin, &addrlen);
      if(new_sock > 0){
        printf("Der Client %s ist verbunden ...\n", inet_ntoa(sin.sin_addr));
      }
      while(strcmp(buffer, "quit\n") != 0){
        printf("Nachricht zum Versenden: ");
        fgets(buffer, BUF, stdin);
        send(new_sock, buffer, strlen(buffer), 0);
        size = recv(new_sock, buffer, BUF-1, 0);
        if(size > 0){
          buffer[size] = '\0';
        }
        printf("Nachricht empfangen: %s\n", buffer);
      }
      close(new_sock);
    }

    // Socket schließen
    close(sock);
    return (EXIT_SUCCESS);
}
