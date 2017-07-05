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
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#define BUF 128
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
int put(char* buffer, int counter);
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
  char line[BUF];

  // Initialisieren vom struct sin mit geeigneten Werten
  int sin_len = sizeof(server);
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(4711);      // 4711 ist die Portangabe

  // Socket erzeugen
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
    printf("Socket %i wurde angelegt!\n", sock);
  } else {
    printf("Socket %i konnte nicht angelegt werden.\n", sock);
  }

  int option = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void *) &option, sizeof(int));

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
    printf("Server bereit.\n");
  }

  unsigned short marker[1]; // aus technischen Gründen

  // Semaphore für den Zugriff auf readcounter
  int mutex = semget(IPC_PRIVATE, 1, IPC_CREAT|0777);
  if(mutex == -1) {
    perror("Die Gruppe mutex konnte nicht angelegt werden!");
    exit(1);
  }
  int db = semget(IPC_PRIVATE, 1, IPC_CREAT|0777);
  if(db == -1) {
    perror("Die Gruppe db konnte nicht angelegt werden!");
    exit(1);
  }

  // Semaphoren auf 1 setzen
  marker[0] = 1;
  semctl(mutex, 1, SETALL, marker);
  semctl(db,    1, SETALL, marker);

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
  KeyValue_Wrapper->readcounter = 0;

  // Werte aus der database.txt in struct Array laden
  FILE *file = fopen("database.txt", "r");
  if (file == NULL) {
    printf("Error opening file!\n");
    exit(1);
  }
  char **result = malloc(100);
  while(fgets(line, sizeof(line), file)) {
    int count = strtoken(line, " ", result, 2);

    printf("Key: %s\n",   result[0]);
    printf("Value: %s", result[1]);
    printf("Counter: %d\n\n", KeyValue_Wrapper->counter);

    strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key,   result[0]);
    strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].value, result[1]);
    KeyValue_Wrapper->counter++;
  }
  fclose(file);

  while(1) {
    new_sock = accept(sock, (struct sockaddr *) &server, &addrlen);

    if(fork() == 0) {

      if(new_sock > 0) {
        printf("Client %s ist verbunden ...\n", inet_ntoa(server.sin_addr));
      }

      // Willkommensnachricht für den Client
      write(new_sock, "***************************************************\n", 52);
      write(new_sock, "*                                                 *\n", 52);
      write(new_sock, "*    Welcome to this fresh Key-Value Server       *\n", 52);
      write(new_sock, "*                                                 *\n", 52);
      write(new_sock, "***************************************************\n", 52);
      write(new_sock, "*                                                 *\n", 52);
      write(new_sock, "*  Folgende Funktionen stehen zur Verfügung :     *\n", 53);
      write(new_sock, "*  - PUT {key} {value} (Key-Value Wert speichern) *\n", 52);
      write(new_sock, "*  - GET {key}         (Value eines Key abfragen) *\n", 52);
      write(new_sock, "*  - DEL {key}         ( Key-Value Wert löschen ) *\n", 53);
      write(new_sock, "*                                                 *\n", 52);
      write(new_sock, "***************************************************\n", 52);

      struct sembuf enter, leave;               // structs für den Semaphor
      enter.sem_num = leave.sem_num = 0;        // Semaphor 0 in der Gruppe
      enter.sem_flg = leave.sem_flg = SEM_UNDO;
      enter.sem_op  = -1;                       // blockieren, DOWN-Operation
      leave.sem_op  = 1;                        // freigeben, UP-Operation

      while(strncmp(buffer, quit, 4) != 0) {
        int size = read(new_sock, buffer, BUF-1);
        if(size > 0) {
          buffer[size] = '\0';
        }

        // GET
        if (strncmp(buffer, stringGet, 3) == 0) {
          semop(mutex, &enter, 1);              // Eintritt in kritischen Bereich
          KeyValue_Wrapper->readcounter += 1;
          if(KeyValue_Wrapper->readcounter == 1) {
            semop(db, &enter, 1);
          }
          semop(mutex, &leave, 1);
          int getValue = get(buffer, KeyValue_Wrapper->counter);
          if(getValue >= 0) {
            write(new_sock, KeyValue_Wrapper->keyValues[getValue].value,
                     sizeof(KeyValue_Wrapper->keyValues[getValue].value));
          } else if(getValue == -1) {
            write(new_sock, "Fehler: Kein Value gefunden!\n", 29);
          }
          semop(mutex, &enter, 1);
          KeyValue_Wrapper->readcounter -= 1;
          if(KeyValue_Wrapper->readcounter == 0) {
            semop(db, &leave, 1);
          }
          semop(mutex, &leave, 1);              // Verlassen des kritischen Bereich

          // PUT
        } else if (strncmp(buffer, stringPut, 3) == 0) {
          semop(db, &enter, 1);                 // Eintritt in kritischen Bereich
          // sleep(10);
          int putValue = put(buffer, KeyValue_Wrapper->counter);
          write(new_sock, "Key gespeichert: ", 18);
          write(new_sock, KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key,
                   sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key));
          write(new_sock, "\n", 1);
          printf("Counter %i\n\n", KeyValue_Wrapper->counter);
          printf("putValue: %i\n", putValue);
          if(putValue == 0){
            write(new_sock, "Key wurde mit neuem Value überschrieben.\n", 42);
          } else {
            KeyValue_Wrapper->counter++;
            write(new_sock, "Erfolgreich gespeichert.\n", 25);
          }
          file = fopen("database.txt", "w+");
          if(file == NULL){
            printf("Error opening file!\n");
            exit(1);
          }
          for (int i = 0; i < KeyValue_Wrapper->counter; i++) {
            fprintf(file, "%s %s", KeyValue_Wrapper->keyValues[i].key,
                                   KeyValue_Wrapper->keyValues[i].value);
          }
          fclose(file);
          semop(db, &leave, 1);                 // Verlassen des kritischen Bereich

          // DELETE
        } else if (strncmp(buffer, stringDel, 3) == 0) {
          semop(db, &enter, 1);                 // Eintritt in kritischen Bereich
          int delValue = delete(buffer, KeyValue_Wrapper->counter);
          KeyValue_Wrapper->counter--;
          if(delValue == 0) {
            write(new_sock, "Value nicht gefunden!\n", 22);
          } else {
            file = fopen("database.txt", "w+");
            if(file == NULL){
              printf("Error opening file!\n");
              exit(1);
            }
            for (int i = 0; i < KeyValue_Wrapper->counter; i++) {
              fprintf(file, "%s %s", KeyValue_Wrapper->keyValues[i].key,
                                     KeyValue_Wrapper->keyValues[i].value);
            }
            fclose(file);
            printf("Erfolgreich gelöscht!\n");
          }
          semop(db, &leave, 1);                 // Verlassen des kritischen Bereich

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
  return i;
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
int put(char* buffer, int counter) {
  char **result = malloc(100);
  printf("PUT Funktion\n");
  int count = strtoken(buffer, " ", result, 3);
  for(int i = 0; i < counter; i++)
    if(strcmp(KeyValue_Wrapper->keyValues[i].key,  result[1]) == 0) {
      strcpy(KeyValue_Wrapper->keyValues[i].value, result[2]);
      printf("Key gefunden und überschrieben %s\n", KeyValue_Wrapper->keyValues[i].value);
      return 0;
    }

    strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key,   result[1]);
    printf("Key gespeichert: %s\n", KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key);
    strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].value, result[2]);
    printf("Value gespeichert: %s", KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].value);
    return 1;
}

// DELETE Funktion
int delete(char* buffer, int counter) {
  int getValue = get(buffer, KeyValue_Wrapper->counter);
  printf("getValue: %i\n", getValue);
  if(getValue >= 0) {
    swap(getValue, KeyValue_Wrapper->counter);
    memset(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].key,
           0, sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].key));
    memset(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].value,
           0, sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].value));
    return 1;
  }
  return 0;
}

void swap(int a, int b) {
  KeyValue temp = KeyValue_Wrapper->keyValues[a];
  KeyValue_Wrapper->keyValues[a]   = KeyValue_Wrapper->keyValues[b-1];
  KeyValue_Wrapper->keyValues[b-1] = temp;
}
