#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int sigCounter = 0, recievedSIGUSR2 = 0, recievedSIGRTMAX = 0;
int toSend, MODE;

void catchSIGUSR1(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid() && sigCounter < toSend){
        sigCounter++;
        if(MODE == 0){
            kill(info->si_pid, sig);
        }
        else{
            union sigval value;
            value.sival_int = MODE;
            sigqueue(info->si_pid, sig, value);
        }
    }else{
        if(MODE == 0){
            kill(info->si_pid, SIGUSR2);
        }
        else{
            union sigval value;
            value.sival_int = MODE;
            sigqueue(info->si_pid, SIGUSR2, value);
        }
    }
}

void catchSIGUSR2(int sig){
    printf("Recieved %d signals back\n", sigCounter);
    exit(0);
}

void catchSIGRTMIN(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid() && sigCounter < toSend){
        sigCounter++;
        kill(info->si_pid, sig);
    }else{
        kill(info->si_pid, SIGRTMAX);
    }
}

void catchSIGRTMAX(int sig){
    printf("Recieved %d signals back\n", sigCounter);
    exit(0);
}

void setupSigCatcher(){
    struct sigaction actSIGUSR1;
    actSIGUSR1.sa_flags = SA_SIGINFO;
    actSIGUSR1.sa_sigaction = &catchSIGUSR1;
    sigaction(SIGUSR1, &actSIGUSR1, NULL);

    struct sigaction actSIGUSR2;
    actSIGUSR2.sa_handler = &catchSIGUSR2;
    sigaction(SIGUSR2, &actSIGUSR2, NULL);

    struct sigaction actSIGRTMIN;
    actSIGRTMIN.sa_flags = SA_SIGINFO;
    actSIGRTMIN.sa_sigaction = &catchSIGRTMIN;
    sigaction(SIGRTMIN, &actSIGRTMIN, NULL);

    struct sigaction actSIGRTMAX;
    actSIGRTMAX.sa_handler = &catchSIGRTMAX;
    sigaction(SIGRTMAX, &actSIGRTMAX, NULL);
}

int main(int argc, char **argv){
    printf("my pid: %d\n", getpid());

    pid_t catcherPID = atoi(argv[1]);
    toSend = atoi(argv[2]);
    char* sendingMode = argv[3];

    setupSigCatcher();

    if(strcmp(sendingMode, "kill") == 0){
        MODE = 0;
        kill(catcherPID, SIGUSR1);
    }else if(strcmp(sendingMode, "sigqueue") == 0){
        MODE = 1;
        union sigval value;
        value.sival_int = 1;
        sigqueue(catcherPID, SIGUSR1, value);
    }else if(strcmp(sendingMode, "sigrt") == 0){
        MODE = 2;
        kill(catcherPID, SIGRTMIN);
    }else{
        printf("wrong argument\n");
    }

    sigset_t tempMask;
    sigfillset(&tempMask);
    sigdelset(&tempMask, SIGUSR1);
    sigdelset(&tempMask, SIGUSR2);
    sigdelset(&tempMask, SIGRTMIN);
    sigdelset(&tempMask, SIGRTMAX);

    while(1){
        sigsuspend(&tempMask);
    }


    return 0;
}
