#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int sock;
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

    close(sock);
    printf("");
    return (EXIT_SUCCESS);
}
