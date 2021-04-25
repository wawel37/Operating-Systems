#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

void printList(const char* sortBy){

    char* toExec;
    //using sudo because of the root mail
    if (strcmp(sortBy, "date") == 0){
        toExec = "echo | sudo mail -H";
    }else if (strcmp(sortBy, "user") == 0){
        toExec = "echo | sudo mail -H | sort -k 3";
    }else{
        printf("Wrong argument!\n");
        exit(1);
    }

    FILE *file = popen(toExec, "r");

    char* buff = NULL;
    size_t buffSize = 0;
    while(getline(&buff, &buffSize, file) != -1){
        printf("%s", buff);
    }
    pclose(file);
    free(buff);
}

void sendMail(const char* user, const char* body, const char* subject){
    char buff[strlen(user) + strlen(body) + strlen(subject) + 30];
    sprintf(buff, "echo %s | sudo mail %s -s %s", body, user, subject);
    FILE* file = popen(buff, "r");
    pclose(file);
}


int main(int argc, char** argv){
    if(argc == 2){
        printList(argv[1]);
    }else if(argc == 4){
        sendMail(argv[1], argv[2], argv[3]);
    }else{
        printf("Wrong number of arguments\n");
    }

    return 0;
}