#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void createProcess(int numOfProcess){
    for(int i = 0; i < numOfProcess; i++){
        if(fork() == 0){
            printf("Child: %d  Parent: %d\n", getpid(), getppid());
            exit(0);
        }
    }
}


int main(int argc, char **argv){
    int N = atoi(argv[1]);
    int status = 0;

    createProcess(N);
    
    while(wait(&status) > 0);
    return 0;
}