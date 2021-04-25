#include "lib.h"

//GLOBAL VARIABLES
Client clients[MAX_CLIENTS];
int serverQueueID = NOT_EXISTING;

void signalHandler(int sig);
void exitHandler();
void initializeServer();
void initializeServerQueue();
void listenToMessages();
void initClient(Message *message);
void stopClient(Message *message);
void listClients(Message *message);
void disconnectClient(Message *message);
void connectClient(Message *message);
int getFirstFreeClientID();
int getIndexOfClient(pid_t pid);
void sendMessage(Message *message, int index);


int main(){
    printf("moj pid: %d\n", getpid());
    initializeServer();
    initializeServerQueue();

    while(true){
        listenToMessages();
    }
    return 0;

}

void exitHandler(){
    if(serverQueueID != NOT_EXISTING){
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].pid != NOT_EXISTING){
                Message message;
                message.type = STOP;
                message.sourcePid = getpid();
                sendMessage(&message, i);
            }
        }
        //TODO
        //Wyrzuca tutaj error, a tak naprawde dziala (nie wiem dlaczego)
        printf("moje serverid to : %d\n", serverQueueID);
        if(msgctl(serverQueueID, IPC_RMID, NULL) == -1){
            printf("error: %d\n", errno);
            perror("Server Queue couldn't be removed!");
        }
    }
}

void signalHandler(int sig){
    exit(1);
}

void initializeServer(){

    //Initializing client array
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].isActive = false;
        clients[i].isAvailable = false;
        clients[i].pairID = NOT_EXISTING;
        clients[i].queue = NOT_EXISTING;
        clients[i].pid = NOT_EXISTING;
    }

    //Initializing normal exit
    if(atexit(exitHandler) != 0){
        perror("Exit function not set!");
        exit(1);
    }

    //Initializing ctrl - c exit
    if(signal(SIGINT, signalHandler) == SIG_ERR){
        perror("Error occurred while setting SIGINT handler");
        exit(1);
    }
}

void initializeServerQueue(){
    key_t serverQueueKey = ftok(HOME_PATH, QUEUE_KEY_SEED);

    if(serverQueueKey == -1){
        perror("Error occurred while generating server queue key");
        exit(1);
    }

    //if the queue already exists
    if((serverQueueID = msgget(serverQueueKey, 0666)) > -1){
        msgctl(serverQueueID, IPC_RMID, NULL);
    }

    //Flags set to return error when a queue with this key already exists
    serverQueueID = msgget(serverQueueKey, IPC_CREAT | IPC_EXCL | 0666);

    if(serverQueueID == -1){
        perror("Error occuredd while reading V system msg queue (probably a queue with this key already exists!)");
        exit(1);
    }
}

void listenToMessages(){
    struct Message message;
    printf("%d\n", serverQueueID);
    if(msgrcv(serverQueueID, &message, MESSAGE_STRUCT_SIZE,0,0) == -1){
        perror("Error while recieving message");
        exit(1);
    }

    switch(message.type){
        case INIT:
            initClient(&message);
            break;
        case STOP:
            stopClient(&message);
            break;
        case LIST:
            listClients(&message);
            break;
        case DISCONNECT:
            disconnectClient(&message);
            break;
        case CONNECT:
            connectClient(&message);
            break;
    }
}

void initClient(Message *message){
    int index = getFirstFreeClientID();
    if(index == FULL_CLIENTS){
        perror("Reached max number of clients!");
        return;
    }

    //int clientQueueKey;
    int clientQueueID;
    sscanf(message->message, "%d", &clientQueueID);
    //printf("clinetkey: %d\n", clientQueueKey);
    // int clientQueueID = msgget(clientQueueKey, 0);
    if(clientQueueID == -1){
        perror("Client queue doesn't exists!");
        return;
    }

    clients[index].isActive = true;
    clients[index].isAvailable = true;
    clients[index].queue = clientQueueID;
    clients[index].pid = message->sourcePid;

    printf("Successfully added new client with pid: %d\n", message->sourcePid);

    Message response;
    sprintf(response.message, "%d", index);
    response.sourcePid = getpid();
    response.type = INIT;
    sendMessage(&response, index);
}

void stopClient(Message *message){
    int index = getIndexOfClient(message->sourcePid);

    if(index == -1){
        perror("Client with that pid isn't initialized yet!");
        return;
    }
    if(clients[index].pairID != NOT_EXISTING){
        //TODO
        //NEED TO HANDLE THIS CASE
    }

    clients[index].isActive = false;
    clients[index].isAvailable = false;
    clients[index].pid = NOT_EXISTING;
    clients[index].pairID = NOT_EXISTING;
    clients[index].queue = NOT_EXISTING;
}

void listClients(Message *message){
    int index = getIndexOfClient(message->sourcePid);

    if(index == -1){
        perror("Client with that pid isn't initialized yet!");
        return;
    }

    Message response;
    response.message[0] = '\0';
    //sprintf(response.message, "%s", "");
    for(int i = 0; i < MAX_CLIENTS; i++){
        char buff[50];
        sprintf(buff, "Client %d: %s\n", i, clients[i].isAvailable ? "Available" : "Not available");
        strcat(response.message, buff);
    }
    response.type = LIST;
    response.sourcePid = getpid();

    sendMessage(&response, index);
}

void disconnectClient(Message *message){
    int index = getIndexOfClient(message->sourcePid);

    if(index == -1){
        perror("Client with that pid isn't initialized yet!");
        return;
    }
    int pairIndex = clients[index].pairID;

    if(clients[index].isAvailable == true){
        perror("Client wasn't connected");
        return;
    }

    if(clients[pairIndex].isAvailable == true){
        perror("Pair client wans't connected");
        return;
    }

    clients[index].isAvailable = true;
    clients[index].pairID = NOT_EXISTING;
    clients[pairIndex].isAvailable = true;
    clients[pairIndex].pairID = NOT_EXISTING;

    Message response;
    sprintf(response.message, "%s", "Disconnected from client");
    response.type = DISCONNECT;
    response.sourcePid = getpid();

    sendMessage(&response, index);
    sendMessage(&response, pairIndex);
}

void connectClient(Message *message){
    int index = getIndexOfClient(message->sourcePid);
    int pairIndex;
    sscanf(message->message, "%d", &pairIndex);
    if(index == -1){
        perror("Client with that pid isn't initialized yet!");
        return;
    }

    if(pairIndex >= MAX_CLIENTS || clients[pairIndex].pid == NOT_EXISTING){
        perror("Pointed client doesn't exists or isn't initialized yet");
        return;
    }

    if(clients[pairIndex].isAvailable == false){
        perror("Pointed client is busy right now");
        return;
    }

    clients[index].isAvailable = false;
    clients[index].pairID = pairIndex;
    clients[pairIndex].isAvailable = false;
    clients[pairIndex].pairID = index;

    Message respond1, respond2;
    sprintf(respond1.message, "%d", clients[index].queue);
    sprintf(respond1.message, "%d", clients[pairIndex].queue);
    respond1.sourcePid = getpid();
    respond2.sourcePid = getpid();
    respond1.type = CONNECT;
    respond2.type = CONNECT;

    sendMessage(&respond1, pairIndex);
    sendMessage(&respond2, index);
}

int getFirstFreeClientID(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].queue == NOT_EXISTING){
            return i;
        }
    }
    return FULL_CLIENTS;
}

int getIndexOfClient(pid_t pid){
    printf("pid: %d\n", pid);
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].pid == pid){
            return i;
        }
    }
    return -1;
}

void sendMessage(Message *message, int index){
    printf("Sending: %s\n", message->message);
    if(msgsnd(clients[index].queue, message, MESSAGE_STRUCT_SIZE, 0) == -1){
        perror("Cannot send messages to client");
        printf("client queue id: %d\t client pid: %d\n", clients[index].queue, clients[index].pid);
    }
}