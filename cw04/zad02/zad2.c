#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void SA_SIGINFO_handler(int sig, siginfo_t *info, void *ucontext){
    printf("Signal number: %d\n", info->si_signo);
    printf("Proccess PID: %d\n", getpid());
    printf("Signal code: %d\n", info->si_code);
    printf("Signal file decriptor: %d\n", info->si_fd);
    printf("Singal error value: %d\n", info->si_errno);
}

void handler(int sig){
    printf("Signal recieved by PID: %d \t Signal number: %d\n", getpid(), sig);
}

void test_SA_SIGINFO(){
    printf("=== SA_SIGINFO ===\n");
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = &SA_SIGINFO_handler;
    sigaction(SIGUSR1, &act, NULL);

    raise(SIGUSR1);
    printf("\n\n");
}

void test_SA_RESETHAND(){
    printf("=== SA_RESETHAND ===\n");
    struct sigaction act;
    act.sa_handler = &handler;
    sigaction(SIGUSR1, &act, NULL);

    
    printf("Raising SIGUSR1 first time without flag!\n");
    raise(SIGUSR1);

    printf("Raising SIGUSR1 second time without flag!\n");
    raise(SIGUSR1);
    /*
        === CHANGING TO SA_RESETHAND ===

        According to docs:
            Restore the signal action to the default upon entry to the
            signal handler.  This flag is meaningful only when
            establishing a signal handler.
        after executing handler first time, our program should be
        terminated due to default SIGUSR1 handling
    */

    act.sa_flags = SA_RESETHAND;
    sigaction(SIGUSR1, &act, NULL);

    printf("Raising SIGUSR1 first time with flag!\n");
    raise(SIGUSR1);

    printf("Raising SIGUSR1 second time with flag!\n");
    raise(SIGUSR1);

    printf("\n\n");
}

void test_SA_NOCLDSTOP(){
    printf("=== SA_NOCLDSTOP ===\n");
    struct sigaction act;
    act.sa_handler = &handler;
    sigaction(SIGCHLD, &act, NULL);


    pid_t childPID;
    if((childPID = fork()) < 0){
        printf("Fork error!\n");
        exit(1);
    }else if(childPID == 0){
        while(1);
    }else{
        printf("Sending SIGSTOP to child process before flag change\n");
        kill(childPID, SIGSTOP);
        sleep(1);
        printf("Sending SIGCONT to child process before flag change\n");
        kill(childPID, SIGCONT);
    }
    kill(childPID, SIGKILL);

    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, NULL);
    
    if((childPID = fork()) < 0){
        printf("Fork error!\n");
        exit(1);
    }else if(childPID == 0){
        while(1);
    }else{
        printf("Sending SIGSTOP to child process after flag change\n");
        kill(childPID, SIGSTOP);
        sleep(1);
        printf("Sending SIGCONT to child process after flag change\n");
        kill(childPID, SIGCONT);
    }
    kill(childPID, SIGKILL);

    printf("\n\n");
}

int main(int argc, char** argv){

    test_SA_SIGINFO();

    test_SA_NOCLDSTOP();

    test_SA_RESETHAND();


    return 0;
}