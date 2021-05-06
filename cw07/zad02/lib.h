#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <unistd.h>

#define SEMAPHORE_SEED getenv("HOME")
#define SEMAPHORE_PROJECT_IDENTIFIER 0
#define FURNANCE_SEED "Furnance.txt"
#define FURNANCE_PROJECT_IDENTIFIER 1
#define TABLE_SEED "Table.txt"
#define TABLE_PROJECT_IDENTIFIER 2
#define TABLE_SEMAPHORE "Table_Semaphore.txt"
#define FURNANCE_SEMAPHORE "Furnance_Semaphore.txt"

#define TABLE_SIZE 5
#define FURNANCE_SIZE 5

#define FURNANCE_INDEX 0
#define TABLE_INDEX 1



typedef struct Table{
    int table[TABLE_SIZE];
    int numberOfPizzasInside;
    int openIndex;
}Table;

typedef struct Furnance{
    int furnance[TABLE_SIZE];
    int numberOfPizzasInside;
    int openIndex;
}Furnance;  

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};
