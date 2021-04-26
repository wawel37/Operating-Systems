#include "lib.h"
#include <errno.h>
bool isConnected = false;
bool serverStop = false;
int serverQueueID = NOT_EXISTING;
int clientQueueID = NOT_EXISTING;
int pairClientQueueID = NOT_EXISTING;
int thisIndex = NOT_EXISTING;
char pairQueueName[50];
char clientQueueName[50];

void signalHandler(int sig);
void exitHandler();
void initializeClient();
void initializeClientQueue();
void initializeServerQueue();
void sendMessage(Message *message, int recieverID);
void setTimer();

//Sending Methods
void initializeConnection();
void stop();
void disconnect();
void list();
void connect(int pairID);
void chat(const char *buff);

//Handlers for received messages
void initHandler(Message *message);
void listHandler(Message *message);
void disconnectHandler(Message *message);
void connectHandler(Message *message);
void stopHandler(Message *message);
void chatHandler(Message *message);

void listenMessages(union sigval sv);
void listenSTDIN();

int main(){
    printf("my pid: %d\n", getpid());
    initializeClient();
    initializeClientQueue();
    initializeServerQueue();
    initializeConnection();
    setTimer();
    listenSTDIN();
    return 0;
}

void listenSTDIN(){
    const int buffSize = 1024;
    char buff[buffSize];
    while(fgets(buff, buffSize, stdin)){

        char *lastChar = strstr(buff, "\n");
        *lastChar = '\0';


        if(strcmp("LIST", buff) == 0){
            list();
        }else if(strcmp("DISCONNECT", buff) == 0){
            disconnect();
        }else if(strcmp("STOP", buff) == 0){
            stop();
        }else if(strstr(buff, "CONNECT") != NULL){
            char separator[3] = " ";
            char* temp;
            temp = strtok(buff, separator);
            int counter = 0;
            while(temp != NULL){
                //Checking if the first word is CONNECT
                if(counter == 0 && strcmp(temp, "CONNECT") != 0){
                    break;
                }
                if(counter == 1){
                    int pairID;
                    sscanf(temp, "%d", &pairID);
                    printf("%d\n", pairID);
                    connect(pairID);
                }
                temp = strtok(NULL, separator);
                counter++;
            }
        }else if(isConnected){
            chat(buff);
        }
    }
}
//DONE
void listenMessages(union sigval sv){
    (void)sv;
    Message message;
    if(mq_receive(clientQueueID, (char*) &message, MAX_MSG_LEN ,NULL) == -1){
        printf("errno: %d\n", errno);
        perror("Error while recieving message!");
        exit(1);
    }

    switch(message.type){
        case INIT:
            initHandler(&message);
            break;
        case LIST:
            printf("dostalem lista\n");
            listHandler(&message);
            break;
        case DISCONNECT:
            disconnectHandler(&message);
            break;
        case CONNECT:
            connectHandler(&message);
            break;
        case STOP:
            stopHandler(&message);
            break;
        case MSG:
            chatHandler(&message);
            break;
    }
    
}
//DONE
void setTimer() {
    timer_t timer;
    struct sigevent event;
    event.sigev_notify = SIGEV_THREAD;
    event.sigev_notify_function = listenMessages;
    event.sigev_notify_attributes = NULL;
    event.sigev_value.sival_ptr = NULL;
    timer_create(CLOCK_REALTIME, &event, &timer);
    struct timespec ten_ms = {0, 10000000};
    struct itimerspec timer_value = {ten_ms, ten_ms};
    timer_settime(timer, 0, &timer_value, NULL);
}
//DONE
void signalHandler(int sig){
    exit(1);
}
//DONE
void exitHandler(){
    if(clientQueueID != NOT_EXISTING){
        if(mq_close(clientQueueID) == -1){
            perror("Client cannot close the queue");
        }
        if(mq_unlink(clientQueueName) == -1){
            perror("Client cannot delete the queue");
        }
    }
    if(!serverStop){
        Message message;
        message.type = STOP;
        message.sourcePid = getpid();
        sendMessage(&message, serverQueueID);
    }
    if(isConnected){
        if(mq_close(pairClientQueueID) == -1){
            perror("Cannot close pair queue");
        }
    }
}
//DONE
void initializeClient(){
    if(atexit(exitHandler) != 0){
        perror("Exit function not set!\n");
        exit(1);
    }

    if(signal(SIGINT, signalHandler) == SIG_ERR){
        perror("Error occurred while setting SIGINT handler\n");
        exit(1);
    }
}
//DONE
void initializeClientQueue(){
    struct mq_attr sAttr;
    sAttr.mq_msgsize = MAX_MSG_LEN;
    sAttr.mq_maxmsg = MAX_MSGS;

    sprintf(clientQueueName, "/client%d", getpid());
   
    clientQueueID = mq_open(clientQueueName, O_RDWR | O_CREAT , 0666, &sAttr);
    if(clientQueueID == -1){
        perror("Error occuredd while reading V system msg queue (probably a queue with this key already exists!)\n");
        exit(1);
    }
}
//DONE
void initializeServerQueue(){
    serverQueueID = mq_open(SERVER_ATTR_QUEUE_NAME, O_WRONLY);

    if(serverQueueID == -1){
        perror("Error occuredd while reading V system msg queue (probably a queue with this key already exists!)\n");
        exit(1);
    }
}
//DONE
void initializeConnection(){
    Message message;

    sprintf(message.message, "%s", clientQueueName);
    message.type = INIT;
    message.sourcePid = getpid();
    sendMessage(&message, serverQueueID);
}
//DONE
void sendMessage(Message *message, int recieverID){
    if(mq_send(recieverID, (char*) message, MAX_MSG_LEN, message->type) == -1){
        perror("Cannot send message to server\n");
        printf("server queue id: %d\n", recieverID);
    }
}
//DONE
void list(){
    Message message;
    
    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = LIST;
    sendMessage(&message, serverQueueID);
}
//DONE
void stop(){
    raise(SIGINT);
}
//DONE
void disconnect(){
    Message message;

    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = DISCONNECT;
    sendMessage(&message, serverQueueID);
    isConnected = false;
    pairClientQueueID = NOT_EXISTING;
}
//DONE
void connect(int pairID){
    Message message;

    sprintf(message.message, "%d", pairID);
    message.type = CONNECT;
    message.sourcePid = getpid();

    printf("Sending pairidx: %d\n", pairID);
    sendMessage(&message, serverQueueID);
}
//DONE
void chat(const char *buff){
    Message message;

    strcpy(message.message, buff);
    message.sourcePid = getpid();
    message.type = MSG;
    sendMessage(&message, pairClientQueueID);
}
//DONE
void initHandler(Message *message){
    sscanf(message->message, "%d", &thisIndex);
    printf("My index: %d\n", thisIndex);
}
//DONE
void listHandler(Message *message){
    printf("%s", message->message);
}
//DONE
void connectHandler(Message *message){
    sscanf(message->message, "%s", &pairQueueName);
    pairClientQueueID = mq_open(pairQueueName, O_WRONLY);

    if(pairClientQueueID == -1){
        perror("Error occuredd while reading msg queue (probably a queue with this key already exists!)\n");
        exit(1);
    }
    printf("Recieved connection to: %d\n", pairClientQueueID);
    isConnected = true;
}
//DONE
void disconnectHandler(Message *message){
    printf("%s\n", message->message);
    isConnected = false;
}
//DONE
void stopHandler(Message *message){
    serverStop = true;
    exit(1);
}
//DONE
void chatHandler(Message *message){
    printf("Pair client: %s\n", message->message);
}