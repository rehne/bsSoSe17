// Verbindung mit telnet 127.0.0.1 4711 herstellen

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#define BUF 1024
#define SEGSIZE sizeof(keyValues)

int counter = 0;

// Struct & Array erzeugen
typedef struct KeyValue_ {
  char key[128];
  char value[128];
} KeyValue;

typedef struct KeyValueWrapper_ {
  KeyValue keyValues[128];
  int counter;
  int readcounter;
} KeyValueWrapper;

struct KeyValueWrapper_ *KeyValue_Wrapper;

int strtoken(char *str, char *separator, char **token, int size);
int get(char* buffer, int counter);
void put(char* buffer, int counter);
int delete(char* buffer, int counter);
void swap(int a, int b);

int main() {
  int sock, new_sock;
  socklen_t addrlen;
  struct sockaddr_in server;
  int server_len = sizeof(server);
  char *quit = "quit";
  char *stringGet = "GET";
  char *stringPut = "PUT";
  char *stringDel = "DEL";
  char *message;
  char buffer[BUF];

  // Initialisieren vom struct sin mit geeigneten Werten
  int sin_len = sizeof(server);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(4711); // 4711 ist die Portangabe

  // Socket erzeugen
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
    printf("Socket %i wurde angelegt!\n", sock);
  } else {
    printf("Socket %i konnte nicht angelegt werden.\n", sock);
  }

  // Socket binden
  if(bind(sock, (struct sockaddr *)& server, sizeof(server)) == 0) {
    printf("Socket %i wurde gebunden!\n", sock);
  } else {
    printf("Der Port ist nicht frei - belegt!\n");
  }

  // Socket lauscht auf eingehende Verbindungen
  if (listen(sock, 5) < 0) {
    perror("Error on listen");
  } else {
    printf("Server bereit\n");
  }

  // Shared Memory, neues Segment anlegen oder auf bestehendes Segment zurückgreifen
  int id = shmget(IPC_PRIVATE, sizeof(KeyValueWrapper), IPC_CREAT|0777);
  if(id < 0) {
    printf("Shared Memory konnte nicht angelegt werden!\n");
  } else {
    printf("Shared Memory ID: %d\n\n", id);
  }

  // Segment an Adressraum des Prozesses anhängen
  KeyValue_Wrapper = shmat(id, 0, 0);
  KeyValue_Wrapper->counter = 0;

  while(1) {
    new_sock = accept(sock, (struct sockaddr *) &server, &addrlen);

    if(fork() == 0) {

      if(new_sock > 0) {
        printf("Client %s ist verbunden ...\n", inet_ntoa(server.sin_addr));
      }

      while(strncmp(buffer, quit, 4) != 0) {
        int size = read(new_sock, buffer, BUF-1);
        if(size > 0) {
          buffer[size] = '\0';
        }

        // GET
        if (strncmp(buffer, stringGet, 3) == 0) {
          int getValue = get(buffer, KeyValue_Wrapper->counter);
          if(getValue >= 0) {
            write(new_sock, KeyValue_Wrapper->keyValues[getValue].value, sizeof(KeyValue_Wrapper->keyValues[getValue].value));
          } else if(getValue == -1) {
            write(new_sock, "Fehler: Kein Value gefunden!\n", 29);
          }

          // PUT
        } else if (strncmp(buffer, stringPut, 3) == 0) {
          put(buffer, KeyValue_Wrapper->counter);
          printf("Counter %i\n", KeyValue_Wrapper->counter );
          KeyValue_Wrapper->counter++;

          // DELETE
        } else if (strncmp(buffer, stringDel, 3) == 0) {
          int delValue = delete(buffer, KeyValue_Wrapper->counter);
          KeyValue_Wrapper->counter--;
          if(delValue == 0){
            write(new_sock, "Value nicht gefunden!\n", 22);
          } else {
            printf("Erfolgreich gelöscht!\n");
          }

          // quit
        } else if(strncmp(buffer, quit, 4) != 0) {
          write(new_sock, "Fehler: Keine gültige Funktion aufgerufen\n", 43);
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
  int i = 0;
  token[0] = strtok(str, separator);
  while(token[i++] && i < size) {
    token[i] = strtok(NULL, separator);
  }
  return (i);
}

// GET Funktion
int get(char* buffer, int counter) {
  char **result = malloc(100);
  printf("GET Funktion\n");
  int count = strtoken(buffer, " ", result, 2);
  for (int i = 0; i < KeyValue_Wrapper->counter; i++) {
    if(strcmp(KeyValue_Wrapper->keyValues[i].key, result[1]) == 0) {
      printf("Key gefunden: %s\n", KeyValue_Wrapper->keyValues[i].value);
      return i;
    }
  }
  return -1;
}

// PUT Funktion
void put(char* buffer, int counter) {
  char **result = malloc(100);
  printf("PUT Funktion\n");
  int count = strtoken(buffer, " ", result, 3);
  for(int i = 0; i < KeyValue_Wrapper->counter; i++) {
    printf("KeyValue: %s\n", KeyValue_Wrapper->keyValues[i].key);
    printf("Result: %s\n", result[1]);
    if(strcmp(KeyValue_Wrapper->keyValues[i].key, result[1]) == 0) {
      strcpy(KeyValue_Wrapper->keyValues[i].value, result[2]);
      printf("Key gefunden und überschrieben %s\n", KeyValue_Wrapper->keyValues[i].value);
    } else {
      strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key, result[1]);
      printf("Key gespeichert: %s\n", KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key);
      strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].value, result[2]);
      printf("Value gespeichert: %s\n", KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].value);
    }
  }
}

// DELETE Funktion
int delete(char* buffer, int counter) {
  int getValue = get(buffer, KeyValue_Wrapper->counter);
  printf("getValue: %i\n", getValue);
  if(getValue >= 0){
    swap(getValue, KeyValue_Wrapper->counter);
    memset(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].key, 0, sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].key));
    memset(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].value, 0, sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].value));
    return 1;
  }
  return 0;
}

void swap(int a, int b){
  KeyValue temp = KeyValue_Wrapper->keyValues[a];
  KeyValue_Wrapper->keyValues[a] = KeyValue_Wrapper->keyValues[b-1];
  KeyValue_Wrapper->keyValues[b-1] = temp;
}
