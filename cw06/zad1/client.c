#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "lib.h"

bool isConnected = false;
int serverQueueID = NOT_EXISTING;
int clientQueueID = NOT_EXISTING;
int pairClientQueueID = NOT_EXISTING;

void exitHandler();
void initializeClient();
void initializeClientQueue();
void initializeServerQueue();
void initializeConnection();
void sendMessage(Message *message);
void stop();
void disconnect();
void list();
void connect();

int main(){
    initializeClient();
    initializeClientQueue();
    initializeServerQueue();
    initializeConnection();


    return 0;
}

void exitHandler(){
    if(clientQueueID != NOT_EXISTING){
        if(msgctl(clientQueueID, IPC_RMID, NULL) == -1){
            perror("Client Queue couldn't be removed!\n");
        }
    }
}

void initializeClient(){
    if(atexit(exitHandler) != 0){
        perror("Exit function not set!\n");
        exit(1);
    }

    if(signal(SIGINT, exitHandler) == SIG_ERR){
        perror("Error occurred while setting SIGINT handler\n");
        exit(1);
    }
}

void initializeClientQueue(){
    key_t clientQueueKey = ftok(HOME_PATH, getpid());

    if(clientQueueKey == -1){
        perror("Error occurred while generating server queue key\n");
        exit(1);
    }

    //Flags set to return error when a queue with this key already exists
    clientQueueID = msgget(clientQueueKey,  IPC_CREAT | IPC_EXCL);

    if(clientQueueID == -1){
        perror("Error occuredd while reading V system msg queue (probably a queue with this key already exists!)\n");
        exit(1);
    }
}

void initializeServerQueue(){
    key_t serverQueueKey = ftok(HOME_PATH, QUEUE_KEY_SEED);

    if(serverQueueKey == -1){
        perror("Error occurred while generating server queue key\n");
        exit(1);
    }

    serverQueueID = msgget(serverQueueKey, 0);

    if(serverQueueID == -1){
        perror("Error occuredd while reading V system msg queue (probably a queue with this key already exists!)\n");
        exit(1);
    }
}

void initializeConnection(){
    Message message;

    sprintf(message.message, "%d", clientQueueID);
    message.type = INIT;
    message.sourcePid = getpid();
    sendMessage(&message);
}

void sendMessage(Message *message){
    if(msgsnd(serverQueueID, message, MESSAGE_STRUCT_SIZE, 0) == -1){
        perror("Cannot send message to server\n");
        printf("server queue id: %d\n", serverQueueID);
    }
}

void list(){
    Message message;
    
    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = LIST;
    sendMessage(&message);
}

void stop(){
    Message message;

    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = STOP;
    sendMessage(&message);
    raise(SIGINT);
}

void disconnect(){
    Message message;

    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = DISCONNECT;
    sendMessage(&message);
    isConnected = false;
    pairClientQueueID = NOT_EXISTING;
}

void connect(int pairID){
    Message message;

    prtinf(message.message, "%d", pairID);
    
}