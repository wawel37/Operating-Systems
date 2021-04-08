#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

int CUSTOM_SIGNAL;

void testIgnore(){
    raise(CUSTOM_SIGNAL);
    printf("Signal in child process ignored successfully!\n");
}

void testPending(){
    sigset_t pendingMask;

    sigpending(&pendingMask);
    int isPending = sigismember(&pendingMask, CUSTOM_SIGNAL);
    printf("== Child ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
}

void testMask(){
    sigset_t pendingMask;
    raise(CUSTOM_SIGNAL);

    sigpending(&pendingMask);
    if(sigismember(&pendingMask, CUSTOM_SIGNAL)){
        printf("== Child ==\n Signal %d was blocked successfully!\n", CUSTOM_SIGNAL);
    }
}

int main(int argc, char **argv){
    CUSTOM_SIGNAL = atoi(argv[2]);
    if(strcmp(argv[1], "ignore") == 0){
        testIgnore();
    }else if(strcmp(argv[1], "pending") == 0){
        testPending();
    }else if(strcmp(argv[1], "mask") == 0){
        testMask();
    }else{
        printf("Invalid argument!\n");
    }


    return 0;
}