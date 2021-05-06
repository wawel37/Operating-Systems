#define _XOPEN_SOURCE 500
#define _POSIX_SOURCE
#include "lib.h"

int N, M;
pid_t *employes;
Furnance *furnance;
Table *table;
sem_t *furnanceSem;
sem_t *tableSem;

void exitHandler();
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
//DONE
void exitHandler(){
    // killing all employes
    for(int i = 0; i < N; i++){
        kill(employes[i], SIGINT);
    }

    munmap(furnance, sizeof(Furnance));
    munmap(table, sizeof(Table));
    shm_unlink(TABLE_SEED);
    shm_unlink(FURNANCE_SEED);

    sem_close(tableSem);
    sem_close(furnanceSem);
    sem_unlink(TABLE_SEMAPHORE);
    sem_unlink(FURNANCE_SEMAPHORE);
    free(employes);
}
//DONE
void initSharedMemory(){
    //FURNANCE

    int furnanceID = shm_open(FURNANCE_SEED, O_RDWR | O_CREAT, 0766);
    
    if(furnanceID == -1){
        perror("Cannot create shared memory for furnance");
        exit(1);
    }
    if(ftruncate(furnanceID, sizeof(Furnance)) < 0){
        perror("Cannot set the shared memory size");
        exit(1);
    }

    furnance = mmap(NULL, sizeof(Furnance), PROT_READ | PROT_WRITE, MAP_SHARED, furnanceID, 0);
    furnance->numberOfPizzasInside = 0;
    furnance->openIndex = 0;
    for(int i = 0; i < FURNANCE_SIZE; i++){
        furnance->furnance[i] = -1;
    }
    
    //TABLE

    int tableID = shm_open(TABLE_SEED, O_RDWR | O_CREAT, 0766);
    if(tableID == -1){
        perror("Cannot open shared memory for table");
        exit(1);
    }
    if(ftruncate(tableID, sizeof(Table)) < 0){
        perror("Cannot set the shared memory size");
        exit(1);
    }
    
    table = mmap(NULL, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, tableID, 0);
    table->numberOfPizzasInside = 0; // ERROR
    table->openIndex = 0;
    for(int i = 0; i < TABLE_SIZE; i++){
        table->table[i] = -1;
    }
}
//DONE
void initSemaphoreSet(){
    furnanceSem = sem_open(FURNANCE_SEMAPHORE, O_RDWR | O_CREAT, 0766, 1);
    if(furnanceSem == (void *)(-1)){
        perror("Cannot open furnance semaphore");
        exit(1);
    }

    tableSem = sem_open(TABLE_SEMAPHORE, O_RDWR | O_CREAT, 0766, 1);
    if(tableSem == (void*)(-1)){
        perror("Cannot open table semaphore");
        exit(1);
    }
}
//DONE
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