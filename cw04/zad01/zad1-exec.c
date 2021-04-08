#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define CUSTOM_SIGNAL SIGUSR1

void runExecFile(char* function){
    char signalArg[10] = {'\0'};
    sprintf(signalArg, "%d", CUSTOM_SIGNAL);
    char *argv[] = {"./exec", function, signalArg, NULL};
    execvp(argv[0], argv);
}

void testIgnore(){
    printf("\n==== Testing Ignored Signals (exec) ====\n");
    /*
    According to unix standard signal list, 
    default action for SIGUSR1 is Termination
    */
    struct sigaction signal;
    signal.sa_handler = SIG_IGN;
    sigaction(CUSTOM_SIGNAL, &signal, NULL);


    raise(CUSTOM_SIGNAL);
    printf("Signal in parent process ignored successfully!\n");
    runExecFile("ignore");
    
    exit(0);
}

void testPending(){
    printf("\n==== Testing Pending Signals (exec) ====\n");
    sigset_t newmask, pendingMask;
    sigemptyset(&newmask);
    sigaddset(&newmask, CUSTOM_SIGNAL);
    sigprocmask(SIG_SETMASK, &newmask, NULL);

    raise(CUSTOM_SIGNAL);


    sigpending(&pendingMask);
    int isPending = sigismember(&pendingMask, CUSTOM_SIGNAL);
    printf("== Parent ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");

    runExecFile("pending");
    exit(0);
}


void testMask(){
    printf("\n==== Testing Masked Signals (exec) ====\n");
    sigset_t pendingMask, newMask;
    sigemptyset(&newMask);
    sigaddset(&newMask, CUSTOM_SIGNAL);
    sigprocmask(SIG_SETMASK, &newMask, NULL);



    raise(CUSTOM_SIGNAL);
    
    sigpending(&pendingMask);
    if(sigismember(&pendingMask, CUSTOM_SIGNAL)){
        printf("== Parent ==\n Signal %d was blocked successfully!\n", CUSTOM_SIGNAL);
    }
    runExecFile("mask");
    exit(0);
}



int main(int argc, char **argv){
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