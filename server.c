// Verbindung mit telnet 127.0.0.1 4711 herstellen

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
    ssize_t size, string;
    struct sockaddr_in sin;
    char *buffer = malloc(BUF);
    char *quit = "quit";
    char *get = "GET";
    char *put = "PUT";
    char *del = "DEL";

    // Initialisieren vom struct sin mit geeigneten Werten
    int sin_len = sizeof(sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(4711); // 4711 ist die Portangabe

    // Socket erzeugen
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0){
      printf("Socket (%i) wurde angelegt!\n", sock);
    }

    // Socket binden
    if(bind(sock, (struct sockaddr *)& sin, sizeof(sin)) == 0){
      printf("Socket (%i) wurde gebunden!\n", sock);
    } else {
      printf("Der Port ist nicht frei - belegt!\n");
    }

    // Socket lauscht auf eingehende Verbindungen
    listen(sock, 5);

    while(1){
      new_sock = accept(sock, (struct sockaddr *) &sin, &addrlen);

      if(new_sock > 0){
        printf("Der Client %s ist verbunden ...\n", inet_ntoa(sin.sin_addr));
      }
      while(strncmp(buffer, quit, 4) != 0){
        string = recv(new_sock, buffer, BUF-1, 0);

        if(size > 0){
          buffer[size] = '\0';
        }

        printf("%zd\n", string-2);

        printf("Nachricht erhalten: %s", buffer);

        // klappt noch nicht.. im buffer steht nicht das, was man in telnet schreibt
        if(strncmp(buffer, get, 3) == 0){
          printf("get klappt\n");





        } else if(strncmp(buffer, put, 3) == 0){
          printf("put klappt\n");






        } else if(strncmp(buffer, del, 3) == 0){
          printf("delete klappt\n");




          
        } else {
          printf("klappt nicht\n");
        }
      }
      close(new_sock);
    }

    // Socket schließen
    close(sock);
    return (EXIT_SUCCESS);
}
