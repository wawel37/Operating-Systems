#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

#define CUSTOM_SIGNAL SIGUSR1

void testingHandler(int signal){
    printf("Recieved signal number: %d \t Parent: %d \t Current: %d\n", signal, getppid(), getpid());
}

void testingMaskHandler(int signal){
    printf("Signal %s wasn't blocked!\n \t PPID = %d \t PID = %d\n", strsignal(signal), getpid(), getppid());
}

void testIgnore(){
    printf("\n==== Testing Ignored Signals (fork) ====\n");
    /*
    According to unix standard signal list, 
    default action for SIGUSR1 is Termination
    */
    struct sigaction signal;
    signal.sa_handler = SIG_IGN;
    sigaction(CUSTOM_SIGNAL, &signal, NULL);
    
    pid_t childPID;
    if((childPID = fork()) < 0){
        printf("Error occured while creating child process\n");
        exit(1);
    }else if (childPID == 0){
        raise(CUSTOM_SIGNAL);
        fprintf(stderr, "Signal in parent process ignored successfully!\n");
        while(wait(NULL) > 0);
    }else{
        raise(CUSTOM_SIGNAL);
        fprintf(stderr, "Signal in child process ignored successfully!\n");
    }
    exit(1);
}

void testHandler(){
    printf("\n==== Testing Handling Signals (fork) ====\n");

    struct sigaction signal;
    signal.sa_handler = &testingHandler;
    sigaction(CUSTOM_SIGNAL, &signal, NULL);

    pid_t childPID;
    if((childPID = fork()) < 0){
        printf("Error occured while creating child process\n");
        exit(1);
    }else if (childPID == 0){
        raise(CUSTOM_SIGNAL);
        printf("Signal in parent process handled successfully!\n");
        while(wait(NULL) > 0);
    }else{
        raise(CUSTOM_SIGNAL);
        printf("Signal in child process handled successfully!\n");
    }
    exit(1);
}

void testPending(){
    printf("\n==== Testing Pending Signals (fork) ====\n");
    sigset_t newmask, pendingMask;
    sigemptyset(&newmask);
    sigaddset(&newmask, CUSTOM_SIGNAL);
    sigprocmask(SIG_SETMASK, &newmask, NULL);

    raise(CUSTOM_SIGNAL);


    pid_t childPID;
    int isPending;
    if((childPID = fork()) < 0){
        printf("Error occured while creating child process\n");
        exit(1);
    }else if (childPID == 0){
        sigpending(&pendingMask);
        isPending = sigismember(&pendingMask, CUSTOM_SIGNAL);
        printf("== Parent ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
        while(wait(NULL) > 0);
    }else{
        sigpending(&pendingMask);
        isPending = sigismember(&pendingMask, CUSTOM_SIGNAL);
        printf("== Child ==\n Signal Pending: %s\n", isPending ? "Yes" : "No");
    }
    exit(0);
}


void testMask(){
    printf("\n==== Testing Masked Signals (fork) ====\n");
    sigset_t pendingMask;
    struct sigaction signal;
    signal.sa_handler = &testingMaskHandler;
    sigemptyset(&signal.sa_mask);
    sigaddset(&signal.sa_mask, CUSTOM_SIGNAL);
    sigprocmask(SIG_SETMASK, &signal.sa_mask, NULL);


    pid_t childPID;
    int isPending;
    if((childPID = fork()) < 0){
        printf("Error occured while creating child process\n");
        exit(1);
    }else if (childPID == 0){
        raise(CUSTOM_SIGNAL);
        
        sigpending(&pendingMask);
        if(sigismember(&pendingMask, CUSTOM_SIGNAL)){
            printf("== Parent ==\n Signal %d was blocked successfully!\n", CUSTOM_SIGNAL);
        }
        while(wait(NULL) < 0);
    }else{
        raise(CUSTOM_SIGNAL);

        sigpending(&pendingMask);
        if(sigismember(&pendingMask, CUSTOM_SIGNAL)){
            printf("== Child ==\n Signal %d was blocked successfully!\n", CUSTOM_SIGNAL);
        }
    }
}



int main(int argc, char **argv){
    if(strcmp(argv[1], "ignore") == 0){
        
        testIgnore();
    }else if(strcmp(argv[1], "pending") == 0){
        testPending();
    }else if(strcmp(argv[1], "handle") == 0){
        testHandler();
    }else if(strcmp(argv[1], "mask") == 0){
        testMask();
    }else{
        printf("Invalid argument!\n");
    }


    return 0;
}