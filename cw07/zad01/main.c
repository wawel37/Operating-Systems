#define _POSIX_SOURCE
#include "lib.h"

int N, M;
pid_t *employes;

void exitHandler();
void freeSharedMemory();
void initSharedMemory();
void initSemaphoreSet();
void initPizzeria();


int main(int argc, char **argv){
    if(argc != 3){
        perror("Wrong number of arguments!");
        return -1;
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);

    if(atexit(exitHandler) == -1){
        perror("Cannot set exit hanlder!");
        return -1;
    }

    initSharedMemory();
    initSemaphoreSet();
    initPizzeria();
    return 0;
}

void exitHandler(){
    // killing all employes
    for(int i = 0; i < N; i++){
        kill(employes[i], SIGINT);
    }

    key_t semKey = ftok(SEMAPHORE_SEED, SEMAPHORE_PROJECT_IDENTIFIER);
    if(semKey == -1){
        perror("Cannot get a semaphore key!");
        return;
    }

    int semID = semget(semKey, 2, 0666 | IPC_CREAT);
    if(semctl(semID, 0, IPC_RMID, NULL) == -1){
        perror("Cannot close semaphore set!");
        return;
    }
    freeSharedMemory();
    free(employes);
}

void freeSharedMemory(){
    //FURNANCE FREE
    key_t furnanceKey = ftok(FURNANCE_SEED, FURNANCE_PROJECT_IDENTIFIER);
    if(furnanceKey == -1){
        perror("Cannot get furnanceKey");
        return;
    }

    int furnanceID = shmget(furnanceKey, sizeof(Furnance), 0666);
    if(furnanceID == -1){
        perror("Cannot get furnanceID");
        return;
    }

    if(shmctl(furnanceID, IPC_RMID, NULL) == -1){
        perror("Furnance shared memory cannot be closed!");
        return;
    }

    //TABLE FREE

    key_t tableKey = ftok(TABLE_SEED, TABLE_PROJECT_IDENTIFIER);
    if(tableKey == -1){
        perror("Cannot get tableKey");
        return;
    }

    int tableID = shmget(tableKey, sizeof(Table), 0666);
    if(tableID == -1){
        perror("Cannot get tableID");
        return;
    }

    if(shmctl(tableID, IPC_RMID, NULL) == -1){
        perror("Table shared memory cannot be closed!");
        return;
    }
}

void initSharedMemory(){
    //FURNANCE
    key_t furnanceKey = ftok(FURNANCE_SEED, FURNANCE_PROJECT_IDENTIFIER);
    if(furnanceKey == -1){
        perror("Cannot get furnanceKey!");
        exit(1);
    }

    int furnanceID = shmget(furnanceKey, sizeof(Furnance), 0666 | IPC_CREAT);
    if(furnanceID == -1){
        perror("Cannot create shared memory for furnance");
        exit(1);
    }

    Furnance *furnance = shmat(furnanceID, NULL, 0);
    furnance->numberOfPizzasInside = 0;
    furnance->openIndex = 0;
    for(int i = 0; i < FURNANCE_SIZE; i++){
        furnance->furnance[i] = -1;
    }

    //TABLE

    key_t tableKey = ftok(TABLE_SEED, TABLE_PROJECT_IDENTIFIER);
    if(tableKey == -1){
        perror("Cannot get tableKey!");
        exit(1);
    }

    int tableID = shmget(tableKey, sizeof(Table), 0666 | IPC_CREAT);
    if(tableID == -1){
        perror("Cannot create shared memory for table");
        exit(1);
    }

    Table *table = shmat(tableID, NULL, 0);
    table->numberOfPizzasInside = 0;
    table->openIndex = 0;
    for(int i = 0; i < TABLE_SIZE; i++){
        table->table[i] = -1;
    }
}

void initSemaphoreSet(){
    //0 - FURNANCE SEMAPHORE
    //1 - TABLE SEMAPHORE
    key_t semKey = ftok(SEMAPHORE_SEED, SEMAPHORE_PROJECT_IDENTIFIER);
    if(semKey == -1){
        perror("Cannot get the Semaphore key");
        exit(1);
    }

    int semID = semget(semKey, 2, 0666 | IPC_CREAT);
    if(semID == -1){
        perror("Cannot create Semaphore Set");
        exit(1);
    }

    union semun arg;
    arg.val = 1;

    if(semctl(semID, 0 ,SETVAL, arg) == -1){
        perror("Cannot set value of Furnance semaphore");
        exit(1);
    }

    if(semctl(semID, 1,SETVAL, arg) == -1){
        perror("Cannot set value of Table semaphore");
        exit(1);
    }
}

void initPizzeria(){
    int employesLength = N + M;
    employes = malloc(sizeof(pid_t) * employesLength);
    pid_t childPID;

    for(int i = 0; i < N; i++){
        if((childPID = fork()) == 0){
            execlp("./cook", "./cook", NULL);
            exit(1);
        }else{
            employes[i] = childPID;
        }
    }
    
    for(int i = N; i < N + M; i++){
        if((childPID = fork()) == 0){
            execlp("./supplier", "./supplier", NULL);
            exit(1);
        }else{
            employes[i] = childPID;
        }
    }

    while(wait(NULL) > 0);
}