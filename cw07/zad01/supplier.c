#include "lib.h"

void blockSem(int semID, int shmIndex);
void unBlockSem(int semID, int shmIndex);
Table *getTable();
int getSemaphoreID();
int takePizzaFromTable(Table *table);
int getRandomIntegerBetween(int a, int b);

int main(){
    srand(time(NULL) ^ (getpid()<<16));

    Table *table = getTable();
    int semaphoreID = getSemaphoreID();

    while(1){
        //Waiting for something to appear on the table
        while(table->numberOfPizzasInside <= 0){
            sleep(1);
        }

        //Reserving the table if there is something to get
        blockSem(semaphoreID, TABLE_INDEX);
        int pizzaType = takePizzaFromTable(table);
        unBlockSem(semaphoreID, TABLE_INDEX);

        //Delivering
        sleep(getRandomIntegerBetween(4,5));
        printf("SUPPLIER: pid: %d \t czas: %ld \t Dostarczam pizze: %d\n", getpid(), time(NULL), pizzaType);

        //Coming back
        sleep(getRandomIntegerBetween(4,5));
    }
}

Table *getTable(){
    key_t tableKey = ftok(TABLE_SEED, TABLE_PROJECT_IDENTIFIER);
    if(tableKey == -1){
        perror("Cannot get table Key");
        exit(1);
    }

    int tableID = shmget(tableKey, sizeof(Table), 0666);
    if(tableID == -1){
        perror("Cannot get table ID");
        exit(1);
    }

    Table *result = shmat(tableID, NULL, 0);
    if(result == (void*)(-1)){
        perror("Cannot get table shared memory");
        exit(1);
    }
    
    return result;
}

int getSemaphoreID(){
    key_t semKey = ftok(SEMAPHORE_SEED, SEMAPHORE_PROJECT_IDENTIFIER);
    if(semKey == -1){
        perror("Cannot get semaphore key");
        exit(1);
    }

    int semID = semget(semKey, 2, 0);
    if(semID == -1){
        perror("Cannot get semaphore ID");
        exit(1);
    }

    return semID;
}

void blockSem(int semID, int shmIndex){
    struct sembuf sops;
    sops.sem_num = shmIndex;
    sops.sem_flg = 0;
    sops.sem_op = -1;
    if(semop(semID, &sops, 1) == -1){
        perror("Cannot block semaphore");
        exit(1);
    }
}

void unBlockSem(int semID, int shmIndex){
    struct sembuf sops;
    sops.sem_num = shmIndex;
    sops.sem_flg = 0;
    sops.sem_op = 1;
    if(semop(semID, &sops, 1) == -1){
        perror("Cannot block semaphore");
        exit(1);
    }
}

int takePizzaFromTable(Table *table){
    int pizzaType = table->table[table->openIndex - 1];
    table->numberOfPizzasInside--;
    printf("SUPPLIER: pid: %d \t czas: %ld \t Pobieram pizze: %d \t Liczba pizz na stole: %d\n", getpid(), time(NULL), pizzaType, table->numberOfPizzasInside);
    return pizzaType;
}

int getRandomIntegerBetween(int a, int b){
    return rand() % (b - a + 1) + a;
}