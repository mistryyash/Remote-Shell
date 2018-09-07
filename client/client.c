#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#define NET_SIZE 1000


int main(int argc, char *argv[]){
    char buffer1[NET_SIZE], buffer2[NET_SIZE];
    char * action;
    int servfd, portNo;
    struct sockaddr_in servAdd;
    FILE *file;

    if(argc != 3){
        printf("Required action: %s <IP Address> <Port Number>\n", argv[0]);
        exit(0);
    }

    if ((servfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    servAdd.sin_family = AF_INET;
    sscanf(argv[2], "%d", &portNo);
    servAdd.sin_port = htons((uint16_t)portNo);

    if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
        fprintf(stderr, " inet_pton() has failed\n");
        exit(2);
    }


    if(connect(servfd, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
        fprintf(stderr, "connect() has failed, exiting\n");
        exit(3);
    }

    fprintf(stderr, "Enter desired action: ");
    fgets(buffer1, sizeof(buffer1), stdin);
    write(servfd, buffer1, strlen(buffer1)+1);
    action = strtok(buffer1, " ");

    while(1){
        read(servfd, buffer2, sizeof(buffer2));
        if (strlen(buffer2) != 0) {
            if (strcmp(action, "get") == 0) {
                char *fileName = strtok(NULL, " ");
                fileName[strlen(fileName) - 1] = '\0';
                file = fopen(fileName, "w");
                fwrite(buffer2, 1, strlen(buffer2), file);
                fprintf(stderr, "Client: File downloaded!\n");
                fclose(file);
                strcpy(buffer2, "");

            }
            if (strcmp(action, "put") == 0) {
                char *fileName = strtok(NULL, " ");
                fileName[strlen(fileName) - 1] = '\0';
                file = fopen(fileName, "r");
                fseek (file, 0, SEEK_END);
                int length = ftell (file);
                fseek (file, 0, SEEK_SET);
                fread (buffer1, 1, length, file);
                write(servfd, buffer1, strlen(buffer1)+1);
                fprintf(stderr, "Client: File uploaded!\n");
                fclose(file);
            }
        }else {
            if (strcmp(action, "quit\n") == 0) {
                close(servfd);
                fprintf(stderr, "Client: Connection Closed!\n");
                exit(0);
            }

        }

        fprintf(stderr, "Client: ");
        fgets(buffer1, sizeof(buffer1), stdin);
        write(servfd, buffer1, strlen(buffer1)+1);
        action = strtok(buffer1, " ");
    }
}
