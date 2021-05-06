#include "lib.h"

void blockSem(sem_t *sem);
void unBlockSem(sem_t *sem);
Furnance *getFurnance();
Table *getTable();
sem_t *getSemaphoreID(const char* path);
int createPizza();
int getRandomIntegerBetween(int a, int b);
void putPizzaInFurnance(int pizzaType, Furnance *furnance);
void takeOutPizzaFromFurnance(int index, Furnance *furnance, Table *table);
void putPizzaOnTable(int pizzaType, Table *table);
void signalHandler(int sig);
//DONE
int main(){
    srand(time(NULL) ^ (getpid()<<16));
    signal(SIGINT, signalHandler);

    Furnance *furnance = getFurnance();
    Table *table = getTable();
    sem_t *furnanceSem = getSemaphoreID(FURNANCE_SEMAPHORE);
    sem_t *tableSem = getSemaphoreID(TABLE_SEMAPHORE);

    while(1){
        int pizzaType = createPizza();
        
        //Getting the furnance reserved for us
        blockSem(furnanceSem);
        while(furnance->numberOfPizzasInside == FURNANCE_SIZE){
            unBlockSem(furnanceSem);
            sleep(0.25);
            blockSem(furnanceSem);
        }

        //When we get access to furnance with empty space
        int openIndex = furnance->openIndex;
        putPizzaInFurnance(pizzaType, furnance);
        unBlockSem(furnanceSem);

        //Waiting for pizza to be done
        sleep(getRandomIntegerBetween(4,5));

        //Taking out the pizza
        blockSem(furnanceSem);
        takeOutPizzaFromFurnance(openIndex, furnance, table);
        unBlockSem(furnanceSem);

        //Getting the table reserved for us
        blockSem(tableSem);
        while(table->numberOfPizzasInside == TABLE_SIZE){
            unBlockSem(tableSem);
            sleep(0.25);
            blockSem(tableSem);
        }

        //When we get access to table with empty space
        openIndex = table->openIndex;
        putPizzaOnTable(pizzaType, table);
        unBlockSem(tableSem);


    }
}
//DONE
Furnance *getFurnance(){
    int furnanceID = shm_open(FURNANCE_SEED, O_RDWR | O_CREAT, 0766);
    if(furnanceID == -1){
        perror("Cannot get furnance ID");
        exit(1);
    }

    Furnance *result = mmap(NULL, sizeof(Furnance), PROT_READ | PROT_WRITE, MAP_SHARED, furnanceID, 0);
    if(result == (void*)(-1)){
        perror("Cannot get furnance shared memory");
        exit(1);
    }
    
    return result;
}
//DONE
Table *getTable(){
    int tableID = shm_open(TABLE_SEED, O_RDWR | O_CREAT, 0766);

    if(tableID == -1){
        perror("Cannot get table ID");
        exit(1);
    }

    Table *result = mmap(NULL, sizeof(Table), PROT_READ | PROT_WRITE, MAP_SHARED, tableID, 0);
    if(result == (void*)(-1)){
        perror("Cannot get table shared memory");
        exit(1);
    }
    
    return result;
}
//DONE
sem_t *getSemaphoreID(const char* path){
    sem_t *result = sem_open(path, O_RDWR | O_CREAT, 0766, 1);
    if(result == (void *)(-1)){
        perror("Cannot open %s semaphore");
        exit(1);
    }
    return result;
}
//DONE
void blockSem(sem_t *sem){
    sem_wait(sem);
}
//DONE
void unBlockSem(sem_t *sem){
    sem_post(sem);
}
//DONE
void signalHandler(int sig){
    sem_unlink(FURNANCE_SEMAPHORE);
    sem_unlink(TABLE_SEMAPHORE);
    shm_unlink(FURNANCE_SEED);
    shm_unlink(TABLE_SEED);
}
//DONE
int createPizza(){
    int result = getRandomIntegerBetween(0,9);
    printf("COOK: pid: %d \t czas: %ld \t Przygotowuje pizze: %d\n", getpid(), time(NULL), result);
    return result;
}
//DONE
int getRandomIntegerBetween(int a, int b){
    return rand() % (b - a + 1) + a;
}
//DONE
void putPizzaInFurnance(int pizzaType, Furnance *furnance){
    furnance->numberOfPizzasInside++;
    furnance->furnance[furnance->openIndex] = pizzaType;
    furnance->openIndex = (furnance->openIndex + 1) % FURNANCE_SIZE;
    printf("COOK: pid: %d \t czas: %ld \t Dodalem pizze: %d \t Liczba pizz w piecu: %d \n", getpid(), time(NULL), pizzaType, furnance->numberOfPizzasInside);
}
//DONE
void takeOutPizzaFromFurnance(int index, Furnance *furnance, Table *table){
    printf("COOK: pid: %d \t czas: %ld \t Wyjmuje pizze: %d \t Liczba pizz w piecu: %d \t Liczba pizz na stole: %d\n", getpid(), time(NULL), furnance->furnance[index], furnance->numberOfPizzasInside - 1, table->numberOfPizzasInside);
    furnance->furnance[index] = -1;
    furnance->numberOfPizzasInside--;
}
//DONE
void putPizzaOnTable(int pizzaType, Table *table){
    table->table[table->openIndex] = pizzaType;
    table->numberOfPizzasInside++;
    table->openIndex = (table->openIndex + 1) % TABLE_SIZE;
    printf("COOK: pid: %d \t czas: %ld \t Ustawiam pizze na stole \t Pizz na stole: %d \n", getpid(), time(NULL), table->numberOfPizzasInside);
}