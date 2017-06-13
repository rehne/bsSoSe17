// Verbindung mit telnet 127.0.0.1 4711 herstellen

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SEGSIZE sizeof(int)
#define BUF 1024

int counter = 0;


// Struct & Array erzeugen
typedef struct KeyValue_ {
  char key[128];
  char value[128];
} KeyValue;
KeyValue keyValues[100];

int strtoken(char *str, char *separator, char **token, int size);
int fget(char* buffer, int counter);
void fput(char* buffer, int counter);
int fdel(char* buffer, int counter);
void deleteSpaces(char* in, char *separator);

int main() {
    int sock, new_sock;
    int id, *shar_mem;
    int pid;
    socklen_t addrlen;
    size_t size, string;
    struct sockaddr_in sin;
    char *buffer = malloc(BUF);
    char *quit = "quit";
    char *get = "GET";
    char *put = "PUT";
    char *del = "DEL";
    char search[10];

    int listenVar;

    // Shared Memory Segmet anfordern, anhängen und auf 0 setzen
    id = shmget(IPC_PRIVATE, SEGSIZE, IPC_CREAT|0777);
    shar_mem = (int *)shmat(id, 0, 0);
    *shar_mem = 0;

    // Initialisieren vom struct sin mit geeigneten Werten
    int sin_len = sizeof(sin);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(4711); // 4711 ist die Portangabe

    // Socket erzeugen
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
      printf("Socket (%i) wurde angelegt!\n", sock);
    }

    // Socket binden
    if(bind(sock, (struct sockaddr *)& sin, sizeof(sin)) == 0) {
      printf("Socket (%i) wurde gebunden!\n", sock);
    } else {
      printf("Der Port ist nicht frei - belegt!\n");
    }

    // Socket lauscht auf eingehende Verbindungen
    listenVar = listen(sock, 5);
    if (listenVar < 0) {
      perror("Error on listen");
    }

    while(strncmp(buffer, quit, 4) != 0) {
      new_sock = accept(sock, (struct sockaddr *) &sin, &addrlen);

      if(fork() == 0) {

        if(new_sock > 0) {
          printf("Der Client %s ist verbunden ...\n", inet_ntoa(sin.sin_addr));
        }

        while(strncmp(buffer, quit, 4) != 0) {
          size = read(new_sock, buffer, BUF-1);

          if(size > 0) {
            buffer[size] = '\0';
          }

          // GET
          if (strncmp(buffer, get, 3) == 0) {
            int temp = fget(buffer, counter);
            if(temp >=0) {
              write(new_sock, keyValues[temp].value, sizeof(keyValues[temp].value));
            } else if(temp == (-1)) {
              //printf( "in GET Ende drin\n");
              write(new_sock, "value not found!\n", 17);
            }

          // PUT
          } else if (strncmp(buffer, put, 3) == 0) {
            fput(buffer, counter);
            printf("Counter %i\n",counter );
            counter++;

          // DELETE
          } else if (strncmp(buffer, del, 3) == 0) {
            printf("delete Funktion aufgerufen\n");
          } else {
            printf("keine gültige Funktion aufgerufen\n");
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

void fput(char* buffer, int counter){
  char **result = malloc(100);
  printf("PUT Funktion Aufgerufen\n");
  int temp = strtoken(buffer, " ", result, 3);
  deleteSpaces(result[1], " ");
  //result[1] = "\0";
  strcpy(keyValues[counter].key, result[1]);
  printf("Key gespeichert: %s\n", keyValues[counter].key);
  strcpy(keyValues[counter].value, result[2]);
  printf("Value gespeichert: %s\n", keyValues[counter].value);
}

int fget(char* buffer, int counter){
  char **result = malloc(100);
  printf("GET Funktion Aufgerufen\n");
  int count = strtoken(buffer, " ", result, 2);

  //printf("keyValues %s\n", keyValues[0].key);
  //printf("result %s\n", result[1]);

  for (int i = 0; i <= counter; i++) {
    if(strcmp(keyValues[i].key, result[1]) == 0){
      printf("Key gefunden: %s\n", keyValues[i].value);
      return i;
    }
  }
  return -1;
}

int fdel(char* buffer, int counter){

}
void deleteSpaces(char* in, char *separator){
    char *pin = in, *out = in;
    while(*pin){
        *out = *pin++;
        out += (*out != *separator);
    }
    *out = '\0';
}
