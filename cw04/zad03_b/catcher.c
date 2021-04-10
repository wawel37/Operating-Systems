#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int sigCounter = -1;
int recievedSIGUSR2 = 0, recievedSIGRTMAX = 0;
pid_t senderPID;
int mode = 0;

void catchSIGUSR1(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid()){
        printf("received signal %d\n", sig);
        sigCounter++;
        senderPID = info->si_pid;
        mode = info->si_value.sival_int;
        if(mode == 0){
            kill(senderPID, sig);
        }
        else{
            union sigval value;
            value.sival_int = mode;
            sigqueue(senderPID, sig, value);
        }
    }
}

void catchSIGUSR2(int sig){
    printf("recieved signal USR2\n");
    recievedSIGUSR2 = 1;
    raise(SIGUSR1);
    if(mode == 0){
        kill(senderPID, sig);
    }
    else{
        union sigval value;
        value.sival_int = mode;
        sigqueue(senderPID, sig, value);
    }
}

void catchSIGRTMIN(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid()){
        sigCounter++;
        senderPID = info->si_pid;
        mode = 2;
        kill(senderPID, sig);
    }
}

void catchSIGRTMAX(int sig){
    recievedSIGRTMAX = 1;
    printf("received sigrtmax\n");
    raise(SIGRTMIN);
    kill(senderPID, sig);
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

void listenForSignals(){
    sigset_t tempMask;
    sigfillset(&tempMask);
    sigdelset(&tempMask, SIGUSR1);
    sigdelset(&tempMask, SIGUSR2);
    sigdelset(&tempMask, SIGRTMIN);
    sigdelset(&tempMask, SIGRTMAX);

    while(recievedSIGUSR2 == 0 && recievedSIGRTMAX == 0){
        sigsuspend(&tempMask);
    }

    printf("Recieved %d basic signals\n", sigCounter);
}



int main(int argc, char** argv){
    setupSigCatcher();
    printf("My PID: %d\n", getpid());
    listenForSignals();
    
    return 0;
}