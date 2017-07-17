#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF 1024

int main(int argc, char *argv[]) {
    int sock, size;
    char server_reply[BUF];
    char buffer[BUF];
    struct sockaddr_in server;

    if(argc != 2){
      printf("<client IP-ADRESSE> erwartet\n");
      return 1;
    }

    // Socket anlegen
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("Socket konnte nicht angelegt werden.\n");
    } else {
      printf("Socket %i wurde angelegt\n", sock);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(4711);
    //server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Socket nach beenden wieder freigeben
    //int option = 1;
    //setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));

    // IP-Adresse in Binärform umwandeln
    if(inet_pton(AF_INET, argv[1], &server.sin_addr) <= 0) {
      printf("Fehler beim konvertieren der IP-Adresse\n");
      return 1;
    }

    // Server verbinden
    if(connect(sock, (struct sockaddr *) &server, sizeof(server)) != 0) {
      printf("keine Verbindung hergestellt\n");
    } else {
      printf("Verbindung mit dem Server %s hergestellt\n",
            inet_ntoa(server.sin_addr));
    }

    while(1){
      fgets(buffer, sizeof(buffer), stdin);
      read(sock, server_reply, sizeof(server_reply));
      write(sock, buffer, strlen(buffer));
      puts(server_reply);
    }
    close(sock);
    return (EXIT_SUCCESS);
}
