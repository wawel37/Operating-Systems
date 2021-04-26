#include "lib.h"

//GLOBAL VARIABLES
Client clients[MAX_CLIENTS];
mqd_t serverQueueID = NOT_EXISTING;

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
//DONE
void exitHandler(){
    if(mq_close(serverQueueID) == -1){
        perror("Server queue couldn't be closed!");
    }
    if(serverQueueID != NOT_EXISTING){
        for(int i = 0; i < MAX_CLIENTS; i++){
            if(clients[i].pid != NOT_EXISTING){
                if(mq_close(clients[i].queue) == -1){
                    perror("client queue couldn't be closed!");
                }
                Message message;
                message.type = STOP;
                message.sourcePid = getpid();
                sendMessage(&message, i);
            }
        }
        if(mq_unlink(SERVER_ATTR_QUEUE_NAME) == -1){
            perror("Server Queue couldn't be removed!");
        }
    }
}
//DONE
void signalHandler(int sig){
    exit(1);
}
//DONE
void initializeServer(){

    //Initializing client array
    for(int i = 0; i < MAX_CLIENTS; i++){
        clients[i].isActive = false;
        clients[i].isAvailable = false;
        clients[i].pairID = NOT_EXISTING;
        clients[i].queue = NOT_EXISTING;
        clients[i].pid = NOT_EXISTING;
        clients[i].queueName[0] = '\0';
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
//DONE
void initializeServerQueue(){
    struct mq_attr sAttr;
    sAttr.mq_msgsize = MAX_MSG_LEN;
    sAttr.mq_maxmsg = MAX_MSGS;

    serverQueueID = mq_open(SERVER_ATTR_QUEUE_NAME, O_RDONLY | O_CREAT, 0660, &sAttr);
    if(serverQueueID == -1){
        perror("Couldn't open server queue");
        exit(1);
    }
}
//DONE
void listenToMessages(){
    struct Message message;
    if(mq_receive(serverQueueID, (char*) &message, MAX_MSG_LEN ,NULL) == -1){ 
            perror("Error while recieving message!");
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
//DONE
void initClient(Message *message){
    int index = getFirstFreeClientID();
    if(index == FULL_CLIENTS){
        perror("Reached max number of clients!");
        return;
    }

    int clientQueueID = mq_open(message->message, O_WRONLY);
    if(clientQueueID == -1){
        perror("Client queue doesn't exists!");
        return;
    }

    clients[index].isActive = true;
    clients[index].isAvailable = true;
    clients[index].queue = clientQueueID;
    clients[index].pid = message->sourcePid;
    strcpy(clients[index].queueName, message->message);

    printf("Successfully added new client with pid: %d\n", message->sourcePid);

    Message response;
    sprintf(response.message, "%d", index);
    response.sourcePid = getpid();
    response.type = INIT;
    sendMessage(&response, index);
}
//DONE
void stopClient(Message *message){
    int index = getIndexOfClient(message->sourcePid);

    if(index == -1){
        perror("Client with that pid isn't initialized yet!");
        return;
    }
    //When stopiing while connected
    int pairIdx = clients[index].pairID;
    if(pairIdx != NOT_EXISTING){
        Message response;
        sprintf(response.message, "%s", "Disconnected from client");
        response.type = DISCONNECT;
        response.sourcePid = getpid();

        clients[pairIdx].isAvailable = true;
        clients[pairIdx].pairID = NOT_EXISTING;

        sendMessage(&response, pairIdx);
    }

    clients[index].isActive = false;
    clients[index].isAvailable = false;
    clients[index].pid = NOT_EXISTING;
    clients[index].pairID = NOT_EXISTING;
    clients[index].queue = NOT_EXISTING;
}
//DONE
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
//DONE
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
//DONE
void connectClient(Message *message){
    int index = getIndexOfClient(message->sourcePid);
    int pairIndex;
    printf("message recieved: %s\n", message->message);
    sscanf(message->message, "%d", &pairIndex);
    if(index == -1){
        perror("Client with that pid isn't initialized yet!");
        return;
    }
    printf("pairidx: %d\n", pairIndex);
    printf("index: %d\n", index);
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
    sprintf(respond1.message, "%s", clients[index].queueName);
    sprintf(respond2.message, "%s", clients[pairIndex].queueName);
    respond1.sourcePid = getpid();
    respond2.sourcePid = getpid();
    respond1.type = CONNECT;
    respond2.type = CONNECT;

    sendMessage(&respond1, pairIndex);
    sendMessage(&respond2, index);
}
//DONE
int getFirstFreeClientID(){
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].queue == NOT_EXISTING){
            return i;
        }
    }
    return FULL_CLIENTS;
}
//DONE
int getIndexOfClient(pid_t pid){
    printf("pid: %d\n", pid);
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(clients[i].pid == pid){
            return i;
        }
    }
    return -1;
}
//DONE
void sendMessage(Message *message, int index){
    printf("Sending: %s\n", message->message);
    if(mq_send(clients[index].queue, (char*) message, MAX_MSG_LEN, message->type) == -1){
        perror("Cannot send messages to client");
    }
}