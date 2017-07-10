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

    server.sin_family = AF_INET;
    server.sin_port = htons(4712);
    //server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Socket anlegen
    if ((sock = socket(AF_INET, SOCK_STREAM, 0) > 0)) {
      printf("Socket %i wurde angelegt\n", sock);
    } else {
      printf("Socket konnte nicht angelegt werden.\n");
    }

    // Socket nach beenden wieder freigeben
    int option = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));

    // Socket binden
    /*
    if(bind(sock, (struct sockaddr *)& server, sizeof(server)) == 0) {
      printf("Socket %i wurde gebunden!\n", sock);
    } else {
      printf("Der Port ist nicht frei - belegt!\n");
    }
    */

    // IP-Adresse in Binärform umwandeln
    if(inet_pton(AF_INET, argv[1], &server.sin_addr.s_addr) <= 0) {
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

    while(strcmp(buffer, "quit\n") != 0){
      fgets(buffer, sizeof(buffer), stdin);
      if(write(sock, buffer, strlen(buffer)) <= 0){
        printf("Fehler beim Schreiben\n");
        return 1;
      } else if(read(sock, server_reply, sizeof(server_reply)) <= 0){
        printf("Fehler beim Lesen\n");
        return 1;
      }
      puts(server_reply);
    }
    return (EXIT_SUCCESS);
}
