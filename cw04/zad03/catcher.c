#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

int sigCounter = 0;
int recievedSIGUSR2 = 0, recievedSIGRTMAX = 0;
pid_t senderPID;
int mode = 0;

void catchSIGUSR1(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid()){
        sigCounter++;
        senderPID = info->si_pid;
        mode = info->si_value.sival_int;
        kill(info->si_pid, SIGUSR1);
    }
}

void catchSIGUSR2(int sig){
    recievedSIGUSR2 = 1;
    raise(SIGUSR1);
}

void catchSIGRTMIN(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid()){
        sigCounter++;
        senderPID = info->si_pid;
        mode = 2;
        kill(info->si_pid, SIGUSR1);
    }
}

void catchSIGRTMAX(int sig){
    recievedSIGRTMAX = 1;
    printf("received sigrtmax\n");
    raise(SIGRTMIN);
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

void sendSignalsBackKill(int sig1, int sig2){
    printf("PID of SENDER: %d\n", senderPID);
    int result;
    for(int i = 0; i < sigCounter; i++){
        //printf("Sending signal %d to pid: %d\n", sig1, senderPID);
        result = kill(senderPID, sig1);
        //printf("Result of sending signal: %d\n", result);
    }
    printf("Result of sig1: %d\n", result);
    result = kill(senderPID, sig2);
    printf("Result of sig2: %d\n", result);
    printf("Sent all signals!\n");
}

void sendSignalsBackSIGQUEUE(){
    union sigval value;
    value.sival_int = mode;
    for(int i = 0; i < sigCounter; i++){
        sigqueue(senderPID, SIGUSR1, value);
    }
    sigqueue(senderPID, SIGUSR2, value);
}

int main(int argc, char** argv){
    setupSigCatcher();
    printf("My PID: %d\n", getpid());
    listenForSignals();
    if(mode == 0){
        sendSignalsBackKill(SIGUSR1, SIGUSR2);
    }else if(mode == 1){
        sendSignalsBackSIGQUEUE();
    }else if(mode == 2){
        sendSignalsBackKill(SIGRTMIN, SIGRTMAX);
    }
    return 0;
}