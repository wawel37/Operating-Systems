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


void catchSIGUSR1(int sig, siginfo_t *info, void *ucontext){
    if(info->si_pid != getpid()){
        sigCounter++;
    }
}

void catchSIGUSR2(int sig){
    recievedSIGUSR2 = 1;
    raise(SIGUSR1);
}

void catchSIGRTMIN(int sig, siginfo_t *info, void *ucontext){
    printf("received sigrtmin\n");
    if(info->si_pid != getpid()){
        sigCounter++;
    }
}

void catchSIGRTMAX(int sig){
    recievedSIGRTMAX = 1;
    raise(SIGRTMIN);
}

void sendSignalsKill(int numOfSignals, pid_t catcherPID){
    sigset_t tempMask;
    sigfillset(&tempMask);
    sigdelset(&tempMask, SIGUSR1);
    for(int i = 0; i < numOfSignals; i++){
        int result = kill(catcherPID, SIGUSR1);
    }
    kill(catcherPID, SIGUSR2);
    sigCounter = 0;
}

void sendSignalsSIGQUEUE(int numOfSignals, pid_t catcherPID){
    union sigval value;
    value.sival_int = 1;
    for(int i = 0; i < numOfSignals; i++){
        sigqueue(catcherPID, SIGUSR1, value);
    }
    sigqueue(catcherPID, SIGUSR2, value);
    sigCounter = 0;
}

void sendSignalsSIGRT(int numOfSignals, pid_t catcherPID){
    for(int i = 0; i < numOfSignals; i++){
        int result = kill(catcherPID, SIGRTMIN);
    }
    kill(catcherPID, SIGRTMAX);
    sigCounter = 0;
    printf("Sent all signals!\n");
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
    printf("Started listening to signals\n");
    while(recievedSIGUSR2 == 0 && recievedSIGRTMAX == 0){
        sigsuspend(&tempMask);
    }
    printf("Recieved %d basic signals\n", sigCounter);
}

void calculateDifferenceOfReceivedAndSent(int numOfSent){
    printf("Difference between number of sent and received: %d\n", numOfSent - sigCounter);
}

int main(int argc, char** argv){
    printf("my pid: %d\n", getpid());

    if(argc != 4){
        printf("wrong number of args!\n");
        return 1;
    }

    pid_t catcherPID = atoi(argv[1]);
    int numOfSignals = atoi(argv[2]);
    char* sendingMode = argv[3];
    setupSigCatcher();

    if(strcmp(sendingMode, "kill") == 0){
        sendSignalsKill(numOfSignals, catcherPID);
    }else if(strcmp(sendingMode, "sigqueue") == 0){
        sendSignalsSIGQUEUE(numOfSignals, catcherPID);
    }else if(strcmp(sendingMode, "sigrt") == 0){
        sendSignalsSIGRT(numOfSignals, catcherPID);
    }else{
        printf("Wrong mode argument!\n");
        return 1;
    }
    listenForSignals();
    calculateDifferenceOfReceivedAndSent(numOfSignals);

    return 0;
}