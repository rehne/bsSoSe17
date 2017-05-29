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

int strtoken(char *str, char *separator, char **token, int size);

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
    char **result = malloc(100);

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

      if(fork() == 0){

              if(new_sock > 0){
                printf("Der Client %s ist verbunden ...\n", inet_ntoa(sin.sin_addr));
              }
              while(strncmp(buffer, quit, 4) != 0){
                string = recv(new_sock, buffer, BUF-1, 0);

                if(size > 0){
                  buffer[size] = '\0';
                }

                printf("Nachricht erhalten: %s", buffer);

                if(strncmp(buffer, get, 3) == 0){
                  printf("GET Funktion !\n");

                } else if(strncmp(buffer, put, 3) == 0){
                  printf("PUT Funktion !\n");

                  int count = strtoken(buffer, " ", result, 3);
                  for (int i = 0; i <= count; i++) {
                    printf("Ergebnis: %s\n", result[i]);
                  }
                } else if(strncmp(buffer, del, 3) == 0){
                  printf("DELETE Funktion !\n");

                } else {
                  printf("klappt nicht\n");
                }
              }
      }

      close(new_sock);
    }

    // Socket schließen
    close(sock);
    return (EXIT_SUCCESS);
}

int strtoken(char *str, char *separator, char **token, int size) {
    int i=0;
    token[0] = strtok(str, separator);
    while(token[i++] && i < size)
        token[i] = strtok(NULL, separator);
    return (i);
}
