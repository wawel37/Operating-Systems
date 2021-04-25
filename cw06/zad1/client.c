#include "lib.h"

bool isConnected = false;
bool serverStop = false;
int serverQueueID = NOT_EXISTING;
int clientQueueID = NOT_EXISTING;
int pairClientQueueID = NOT_EXISTING;
int thisIndex = NOT_EXISTING;

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

//Handlers for received messages
void initHandler(Message *message);
void listHandler(Message *message);
void disconnectHandler(Message *message);
void connectHandler(Message *message);
void stopHandler(Message *message);

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
        }else if(strstr()){
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
        }
    }
}

void listenMessages(union sigval sv){
    (void)sv;
    Message message;
    if(msgrcv(clientQueueID, &message, MESSAGE_STRUCT_SIZE, 0, IPC_NOWAIT) >= 0){
        if(isConnected){

        }else{
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
            }
        }
    }
}

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

void signalHandler(int sig){
    exit(1);
}

void exitHandler(){
    if(clientQueueID != NOT_EXISTING){
        if(msgctl(clientQueueID, IPC_RMID, NULL) == -1){
            perror("Client Queue couldn't be removed!\n");
        }
    }
    if(!serverStop){
        Message message;
        message.type = STOP;
        message.sourcePid = getpid();
        sendMessage(&message, serverQueueID);
    }
}

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

void initializeClientQueue(){
    key_t clientQueueKey = ftok(HOME_PATH, getpid());

    if(clientQueueKey == -1){
        perror("Error occurred while generating server queue key\n");
        exit(1);
    }

    if((clientQueueID = msgget(clientQueueKey, 0666)) > -1){
        msgctl(clientQueueID, IPC_RMID, NULL);
    }
    //Flags set to return error when a queue with this key already exists
    clientQueueID = msgget(clientQueueKey,  IPC_CREAT | IPC_EXCL | 0666);

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
    sendMessage(&message, serverQueueID);
}

void sendMessage(Message *message, int recieverID){
    if(msgsnd(recieverID, message, MESSAGE_STRUCT_SIZE, 0) == -1){
        perror("Cannot send message to server\n");
        printf("server queue id: %d\n", recieverID);
    }
}

void list(){
    Message message;
    
    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = LIST;
    sendMessage(&message, serverQueueID);
}

void stop(){
    raise(SIGINT);
}

void disconnect(){
    Message message;

    sprintf(message.message, "%d", clientQueueID);
    message.sourcePid = getpid();
    message.type = DISCONNECT;
    sendMessage(&message, serverQueueID);
    isConnected = false;
    pairClientQueueID = NOT_EXISTING;
}

void connect(int pairID){
    Message message;

    printf(message.message, "%d", pairID);
    message.type = CONNECT;
    message.sourcePid = getpid();

    sendMessage(&message, serverQueueID);
}

void initHandler(Message *message){
    sscanf(message->message, "%d", &thisIndex);
}

void listHandler(Message *message){
    printf("%s", message->message);
}

void connectHandler(Message *message){
    sscanf(message->message, "%d", &pairClientQueueID);
    isConnected = true;
}

void disconnectHandler(Message *message){
    printf("%s\n", message->message);
    isConnected = false;
}

void stopHandler(Message *message){
    serverStop = true;
    exit(1);
}