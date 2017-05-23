#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF 1024

int main(int argc, char **argv) {
    int sock, size;
    char *buffer = malloc(BUF);
    struct sockaddr_in sin;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0) > 0)) {
        printf("Socket %i wurde angelegt\n", sock);
    } else {
        printf("Socket konnte nicht angelegt werden.\n");
    }

    int sin_len = sizeof(sin);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(1500);

    if(connect(sock, (struct sockaddr *) &sin, sizeof(sin)) == 0){
      printf("Verbindung mit dem Server %s hergestellt\n",
              inet_ntoa(sin.sin_addr));
    } else {
      printf("keine Verbindung hergestellt\n");
    }

    while(strcmp(buffer, "quit\n") != 0){
      size = recv(sock, buffer, BUF-1, 0);
      if(size > 0){
        buffer[size] = '\0';
      }
      printf("Nachricht erhalten: %s\n", buffer);
      if(strcmp(buffer, "quit\n")){
        printf("Nachricht zum Versenden: ");
        fgets(buffer, BUF, stdin);
        send(sock, buffer, strlen(buffer), 0);
      }
    }

    close(sock);
    return (EXIT_SUCCESS);
}
