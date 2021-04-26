#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <mqueue.h>
#include <fcntl.h>


#define MAX_CLIENTS 10
#define HOME_PATH getenv("HOME")
#define MAX_MSG_LEN 1000
#define MAX_MSGS 20
#define MESSAGE_STRUCT_SIZE MAX_MSG_LEN + sizeof(pid_t)
#define QUEUE_KEY_SEED 0x099
#define NOT_EXISTING -1
#define FULL_CLIENTS -2
#define SERVER_ATTR_QUEUE_NAME "/server_name3"

#define INIT 1
#define STOP 2
#define DISCONNECT 3
#define LIST 4
#define CONNECT 5
#define MSG 6

typedef struct Client{
    bool isActive;
    bool isAvailable;
    mqd_t queue;
    char queueName[64];
    int pairID;
    pid_t pid;
}Client;

typedef struct Message{
    long type;
    pid_t sourcePid;
    char message[MAX_MSG_LEN];
}Message;