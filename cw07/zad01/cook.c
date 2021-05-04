#include "lib.h"

void blockSem(int semID, int shmIndex);
void unBlockSem(int semID, int shmIndex);
Furnance *getFurnance();
Table *getTable();
int getSemaphoreID();
int createPizza();
int getRandomIntegerBetween(int a, int b);
void putPizzaInFurnance(int pizzaType, Furnance *furnance);
void takeOutPizzaFromFurnance(int index, Furnance *furnance, Table *table);
void putPizzaOnTable(int pizzaType, Table *table);
void signalHandler(int sig);

int main(){
    srand(time(NULL) ^ (getpid()<<16));

    Furnance *furnance = getFurnance();
    Table *table = getTable();
    int semaphoreID = getSemaphoreID();

    while(1){
        int pizzaType = createPizza();
        
        //Getting the furnance reserved for us
        blockSem(semaphoreID, FURNANCE_INDEX);
        while(furnance->numberOfPizzasInside == FURNANCE_SIZE){
            unBlockSem(semaphoreID, FURNANCE_INDEX);
            sleep(0.25);
            blockSem(semaphoreID, FURNANCE_INDEX);
        }

        //When we get access to furnance with empty space
        int openIndex = furnance->openIndex;
        putPizzaInFurnance(pizzaType, furnance);
        unBlockSem(semaphoreID, FURNANCE_INDEX);

        //Waiting for pizza to be done
        sleep(getRandomIntegerBetween(4,5));

        //Taking out the pizza
        blockSem(semaphoreID, FURNANCE_INDEX);
        takeOutPizzaFromFurnance(openIndex, furnance, table);
        unBlockSem(semaphoreID, FURNANCE_INDEX);

        //Getting the table reserved for us
        blockSem(semaphoreID, TABLE_INDEX);
        while(table->numberOfPizzasInside == TABLE_SIZE){
            unBlockSem(semaphoreID, TABLE_INDEX);
            sleep(0.25);
            blockSem(semaphoreID, TABLE_INDEX);
        }

        //When we get access to table with empty space
        openIndex = table->openIndex;
        putPizzaOnTable(pizzaType, table);
        unBlockSem(semaphoreID, TABLE_INDEX);


    }
}

Furnance *getFurnance(){
    key_t furnanceKey = ftok(FURNANCE_SEED, FURNANCE_PROJECT_IDENTIFIER);
    if(furnanceKey == -1){
        perror("Cannot get furnance Key");
        exit(1);
    }

    int furnanceID = shmget(furnanceKey, sizeof(Furnance), 0666);
    if(furnanceID == -1){
        perror("Cannot get furnance ID");
        exit(1);
    }

    Furnance *result = shmat(furnanceID, NULL, 0);
    if(result == (void*)(-1)){
        perror("Cannot get furnance shared memory");
        exit(1);
    }
    
    return result;
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

int createPizza(){
    int result = getRandomIntegerBetween(0,9);
    printf("COOK: pid: %d \t czas: %ld \t Przygotowuje pizze: %d\n", getpid(), time(NULL), result);
    return result;
}

int getRandomIntegerBetween(int a, int b){
    return rand() % (b - a + 1) + a;
}

void putPizzaInFurnance(int pizzaType, Furnance *furnance){
    furnance->numberOfPizzasInside++;
    furnance->furnance[furnance->openIndex] = pizzaType;
    furnance->openIndex = (furnance->openIndex + 1) % FURNANCE_SIZE;
    printf("COOK: pid: %d \t czas: %ld \t Dodalem pizze: %d \t Liczba pizz w piecu: %d \n", getpid(), time(NULL), pizzaType, furnance->numberOfPizzasInside);
}

void takeOutPizzaFromFurnance(int index, Furnance *furnance, Table *table){
    printf("COOK: pid: %d \t czas: %ld \t Wyjmuje pizze: %d \t Liczba pizz w piecu: %d \t Liczba pizz na stole: %d\n", getpid(), time(NULL), furnance->furnance[index], furnance->numberOfPizzasInside - 1, table->numberOfPizzasInside);
    furnance->furnance[index] = -1;
    furnance->numberOfPizzasInside--;
}

void putPizzaOnTable(int pizzaType, Table *table){
    table->table[table->openIndex] = pizzaType;
    table->numberOfPizzasInside++;
    table->openIndex = (table->openIndex + 1) % TABLE_SIZE;
    printf("COOK: pid: %d \t czas: %ld \t Ustawiam pizze na stole \t Pizz na stole: %d \n", getpid(), time(NULL), table->numberOfPizzasInside);
}