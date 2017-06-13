#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#define SEGSIZE sizeof(keyValues)

//  creating struct to store keys and values
	typedef struct KeyValue_ {
		char key[100];
		char value[100];
        
	} KeyValue;

typedef struct KeyValueWrapper {
    KeyValue keyValues[100];
    int counter;
} KEYVALUEWRAPPER;

struct KeyValueWrapper *KeyValue_Wrapper;

int strtoken(char *str, char *separator, char **token, int size);
void put(char* buffer, int counter);
int get(char* buffer, int counter);
int del(char* buffer, int counter);
void swap(int a, int b);
void deleteSpaces(char* in, char *separator);

int main(){
	int sock, fileDescriptor;
	socklen_t addrlen;
	char *quit = "quit";
	char *qget = "GET";
	char *qput = "PUT";
	char *qdel = "DEL";
	char *message;
	char buffer[2000];
    int size;
	struct sockaddr_in server;
	int server_len = sizeof(server);
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(4711);
    
    //printf("%lu\n", sizeof(keyValues));
	

	//creat socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock > 0) printf("Socket %i wurde angelegt\n", sock);
 
	// bind socket to adress
 
	if(bind(sock, (struct sockaddr *) &server, server_len) == 0) {
	printf("socket %i wurde gebunden\n",sock);
	} 
	else printf("port belegt\n");
 
	//listen to connection
	listen(sock, 5);

	//sheared memory, Neues Segment anlegen oder auf bestehendes Segment zugreifen
    int id = shmget(IPC_PRIVATE, sizeof(KEYVALUEWRAPPER), IPC_CREAT|0777);
    if (id < 0) {
        printf("Shared Memory konnte nicht angelegt werden!");
    }
    printf("%d\n", id);

	//Segment an Adressraum des aufrufenden Prozesses anhängen (attach)
    KeyValue_Wrapper = shmat(id, 0, 0);
    
    KeyValue_Wrapper->counter = 0;
        
    
    
    
    
	while (1) {
 	fileDescriptor = accept(sock,(struct sockaddr *) &server, &addrlen);
        
      	if(fork() == 0) {
            
			//shmdt(shar_mem);
			//shmctl(id, IPC_RMID, 0);
            

			if(fileDescriptor > 0) printf("Der Client %s ist verbunden ...\n", inet_ntoa(server.sin_addr));
			while(strncmp(buffer, quit, 4) != 0) {
				int size = read(fileDescriptor, buffer, 1999);
                
                if(size >0){
                    buffer[size] = '\0';
                }
			
				//put
				if(strncmp(buffer, qput, 3) == 0){
					put(buffer, KeyValue_Wrapper->counter);
					//increase counter by 1 so the next tume put gets used, we move into the next position in our array
					KeyValue_Wrapper->counter++;
					printf("Counter = %i \n", KeyValue_Wrapper->counter);
				}

				//get
				else if(strncmp(buffer, qget, 3) == 0){
					int getValue = get(buffer, KeyValue_Wrapper->counter);
                    if(getValue >=0){
                        write(fileDescriptor, KeyValue_Wrapper->keyValues[getValue].value, sizeof(KeyValue_Wrapper->keyValues[getValue].value));
                        
                    }
                    else if(getValue == (-1)) {
                        //printf( "in GET Ende drin\n");
                    write(fileDescriptor, "value not found!\n", 16);
                    }
				}

				//del
				else if(strncmp(buffer, qdel, 3) == 0){
                    printf("ALTER INHALT: \n");
                    for(int i = 0; i < KeyValue_Wrapper->counter; i++){
                    //printf("%s - %s \n", keyValues[i].key, keyValues[i].value);
                    }
                    int delValue = del(buffer, KeyValue_Wrapper->counter);
                    KeyValue_Wrapper->counter--;
                    if(delValue == 0){
                        printf("DEL ERREICHT");
                        write(fileDescriptor, "value not found!", 16);
						}
                    else{
                        //printf("%s - %s\n", keyValues[delValue].key, keyValues[delValue].value);
                        printf("Erfolgreich gelöscht!\n");
                        printf("NEUER INHALT: \n");
                        for(int i = 0; i < KeyValue_Wrapper->counter; i++){
                            //printf("%s - %s \n", keyValues[i].key, keyValues[i].value);
                        }
						
                    }
                }
        
   			
				else {
					if(strncmp(buffer, quit, 4) != 0){
						write(fileDescriptor, "no such function", 16);
					}
				}
			}
		}
		close(fileDescriptor);
        //shmdt(KeyValue);
	}
	close(sock);
    //shmctl(id, IPC_RMID, 0);
	return (EXIT_SUCCESS);

}

int strtoken(char *str, char *separator, char **token, int size){
	int i = 0;
	token[0] = strtok(str, separator);
    while(token[i++] && i < size) token[i] = strtok(NULL, separator);
    return(i);
}

void put(char* buffer, int counter){
	char **str = malloc(100);

	printf("PUT funktion\n");
	//strtoken splits the client input into (3) strings and stores them in a array called "str", in this case the GET command and the key
	int test = strtoken(buffer, " ", str, 3);
    
    deleteSpaces(str[1], " ");
    // take second argument of client input (str[1]) and save it in our array in first position, since counter is at 0
	strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].key, str[1]);
	printf("Key saved %s\n", str[1]);
	strcpy(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter].value, str[2]);
	printf("value saved %s\n", str[2]);

}

int get(char* buffer, int counter){
    char **str = malloc(100);
    printf("GET funktion\n");
    // same ase aboove, this time 2 strings
    int test = strtoken(buffer, " ", str , 2);
    int i = 0;
    printf(" str1 : %s\n", str[1]);
    // this loop runs through the struct containing keys, if strcmp finds a match we give out the value of found key
    for(int i= 0; i < KeyValue_Wrapper->counter; i++){
        if(strcmp(KeyValue_Wrapper->keyValues[i].key, str[1])==0){
            return (i);
        }
        
    }
    return (-1);
}


int del(char* buffer, int counter){
    int getValue = get(buffer, KeyValue_Wrapper->counter);
    //printf("Get Funktion in Del erreicht\n");
    printf("%i\n", getValue);
    if(getValue >= 0){
        //printf("in der if drinen\n");
        //printf("%s\n",keyValues[getValue].key);
        swap(getValue, KeyValue_Wrapper->counter);
        memset(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].key, 0, sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].key));
        memset(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].value, 0, sizeof(KeyValue_Wrapper->keyValues[KeyValue_Wrapper->counter-1].value));
        return(1);
    }
    
    
	return (0);

}

void swap(int a, int b){
    KeyValue temp = KeyValue_Wrapper->keyValues[a];
    KeyValue_Wrapper->keyValues[a] = KeyValue_Wrapper->keyValues[b-1];
    KeyValue_Wrapper->keyValues[b-1] = temp;

}

void deleteSpaces(char* in, char *separator){
    /*char *out;
    int x = strlen(in);
    printf("strlen : %i\n", j );
    for(int y = 0; y <= x; y++ ){
        if(in[y] != separator){
            out[x++] = in[y];
        }
    }*/
    
    char *pin = in, *out = in;
    while(*pin){
        *out = *pin++;
        out += (*out != *separator);
    }
    *out = '\0';
    
}

