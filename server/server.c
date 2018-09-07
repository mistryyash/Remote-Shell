#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

void child(int);

int main(int argc, char *argv[]){
    int sd, client, portNumber;
    struct sockaddr_in servAdd;
    
    if(argc != 2){
        printf("Call model: %s <Port Number>\n", argv[0]);
        exit(0);
    }
    
    sd = socket(AF_INET, SOCK_STREAM, 0);
    servAdd.sin_family = AF_INET;
    servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &portNumber);
    servAdd.sin_port = htons((uint16_t)portNumber);
    
    bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd));
    listen(sd, 5);
    
    while(1){
        client = accept(sd, NULL, NULL);
        printf("Got a client\n");
        
        if(!fork()){
            char buffer[10000], handleCommand[10000];
            FILE *file;
            char *cmd;
            
            while(1){
                read(client, buffer, sizeof(buffer));
                if (strlen(handleCommand) == 0) {
                    cmd = strtok(buffer, " ");
                }
                if (strcmp(cmd,"quit\n") == 0) {
                    close(client);
                    fprintf(stderr, "Server: Connection Closed!\n");
                    exit(0);
                }
                if (strcmp(cmd, "get") == 0) {
                    char *fileName = strtok(NULL, " ");
                    fileName[strlen(fileName) - 1] = '\0';
                    file = fopen(fileName, "r");
                    fseek (file, 0, SEEK_END);
                    int length = ftell (file);
                    fseek (file, 0, SEEK_SET);
                    fread (buffer, 1, length, file);
                    fprintf(stderr, "Server: Sending file...\n");
                    write(client, buffer, sizeof(buffer)+1);
                    fclose (file);
                }
                if (strcmp(cmd, "put") == 0) {
                    strcat(handleCommand, cmd);
                    strcat(handleCommand, " ");
                    char *fileName = strtok(NULL, " ");
                    fileName[strlen(fileName) - 1] = '\0';
                    strcat(handleCommand, fileName);
                    write(client, handleCommand, strlen(handleCommand)+1);
                } else {
                    if (strlen(buffer) != 0) {
                        cmd = strtok(handleCommand, " ");
                        char *fileName = strtok(NULL, " ");
                        file = fopen(fileName, "w");
                        fwrite(buffer, 1, strlen(buffer), file);
                        fprintf(stderr, "Server: File downloaded successfully!\n");
                        fclose(file);
                        strcpy(handleCommand, "");
                    }
                }
            }
        }
        
        close(client);
    }
}

void child(int sd){
    
}
