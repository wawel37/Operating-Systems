#include "lib.h"

void blockSem(sem_t *sem);
void unBlockSem(sem_t *sem);
Table *getTable();
sem_t *getSemaphoreID(const char* path);
int takePizzaFromTable(Table *table);
int getRandomIntegerBetween(int a, int b);

int main(){
    srand(time(NULL) ^ (getpid()<<16));

    Table *table = getTable();
    sem_t *tableSem = getSemaphoreID(TABLE_SEMAPHORE);

    while(1){
        //Waiting for something to appear on the table
        while(table->numberOfPizzasInside <= 0){
            sleep(1);
        }

        //Reserving the table if there is something to get
        blockSem(tableSem);
        int pizzaType = takePizzaFromTable(table);
        unBlockSem(tableSem);

        //Delivering
        sleep(getRandomIntegerBetween(4,5));
        printf("SUPPLIER: pid: %d \t czas: %ld \t Dostarczam pizze: %d\n", getpid(), time(NULL), pizzaType);

        //Coming back
        sleep(getRandomIntegerBetween(4,5));
    }
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
int takePizzaFromTable(Table *table){
    int pizzaType = table->table[table->openIndex - 1];
    table->numberOfPizzasInside--;
    printf("SUPPLIER: pid: %d \t czas: %ld \t Pobieram pizze: %d \t Liczba pizz na stole: %d\n", getpid(), time(NULL), pizzaType, table->numberOfPizzasInside);
    return pizzaType;
}
//DONE
int getRandomIntegerBetween(int a, int b){
    return rand() % (b - a + 1) + a;
}