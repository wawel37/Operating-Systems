#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define NO_REINDEER 9
#define NO_ELFS 10
#define ELFS_BORDER 3
#define REINDEERS_BORDER 9

int waitingElfs = 0;
int waitingReinDeers = 0;

pthread_mutex_t santaMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santaCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elfMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elfCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elfSolvingMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elfSolvingCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reinDeerMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reinDeerCond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reinDeerSolvingMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reinDeerSolvingCond = PTHREAD_COND_INITIALIZER;

int getRandomIntBetween(int a, int b){
    srand(time(NULL) ^ (pthread_self() << 16));
    return rand() % (b - a + 1) + a;
}

void *santaFunction(void *arg){
    pthread_t selfID = pthread_self();
    printf("Mikolaj spi: %ld ID\n", selfID);
    
    int delivered = 0;
    while(delivered < 3){
        pthread_mutex_lock(&santaMutex);
        pthread_cond_wait(&santaCond, &santaMutex);

        printf("Mikolaj: Budze sie %ld ID\n", selfID);

        pthread_mutex_lock(&reinDeerMutex);
        if(waitingReinDeers == REINDEERS_BORDER){
            waitingReinDeers = 0;
            printf("Mikolaj: dostaczam zabawki %ld ID\n", selfID);
            delivered++;
            sleep(getRandomIntBetween(2,4));
            pthread_mutex_lock(&reinDeerSolvingMutex);
            pthread_cond_broadcast(&reinDeerSolvingCond);
            pthread_mutex_unlock(&reinDeerSolvingMutex);
            pthread_mutex_unlock(&reinDeerMutex);
        }else{
            pthread_mutex_unlock(&reinDeerMutex);
            printf("Mikolaj: Rozwiazuje problemy elfow\n");
            sleep(getRandomIntBetween(1,2));
            pthread_mutex_lock(&elfSolvingMutex);
            pthread_mutex_lock(&elfMutex);
            waitingElfs = 0;
            pthread_cond_broadcast(&elfCond);
            pthread_cond_broadcast(&elfSolvingCond);
            pthread_mutex_unlock(&elfSolvingMutex);
            pthread_mutex_unlock(&elfMutex);
        }

        printf("Mikolaj znow zasypia\n");
        pthread_mutex_unlock(&santaMutex);
    }
    printf("Job's done!\n");
    return NULL;
}

void *reindeerFunction(void *arg){
    pthread_t selfID = pthread_self();

    while(1){
        sleep(getRandomIntBetween(5,10));

        pthread_mutex_lock(&reinDeerMutex);
        waitingReinDeers++;
        printf("Renifer: czekaj %d reniferow na Mikolaja. %ld ID\n", waitingReinDeers, selfID);

        if(waitingReinDeers == REINDEERS_BORDER){
            printf("Renifer: budze Mikolaja. %ld ID\n", selfID);
            pthread_mutex_unlock(&reinDeerMutex);
            pthread_mutex_lock(&santaMutex);
            pthread_cond_broadcast(&santaCond);
            pthread_mutex_unlock(&santaMutex);
        }else{
            pthread_mutex_unlock(&reinDeerMutex);
        }

        pthread_mutex_lock(&reinDeerSolvingMutex);
        pthread_cond_wait(&reinDeerSolvingCond, &reinDeerSolvingMutex);
        pthread_mutex_unlock(&reinDeerSolvingMutex);

        sleep(getRandomIntBetween(2,4));
    }
    return NULL;
}

void *elfFunction(void *arg){
    pthread_t selfID = pthread_self();

    while(1){
        //sleeping
        sleep(getRandomIntBetween(2,5));

        //Checking if the value of waiting elfs is less than 3, if not, waits for it to decrement
        pthread_mutex_lock(&elfMutex);
        while(waitingElfs == ELFS_BORDER){
            printf("Elf: %lu czeka na powrot elfow\n", selfID);
            pthread_cond_wait(&elfCond, &elfMutex);
        }
        
        waitingElfs++;
        printf("Elf: czeka %d na mikolaja, %ld ID\n", waitingElfs, selfID);

        if(waitingElfs == 3){
            printf("Elf: wybudzam Mikolaja, %ld ID\n", selfID);
            pthread_mutex_unlock(&elfMutex);
            pthread_mutex_lock(&santaMutex);
            pthread_cond_broadcast(&santaCond);
            pthread_mutex_unlock(&santaMutex);
            
        }else{
            pthread_mutex_unlock(&elfMutex);
        }

        pthread_mutex_lock(&elfSolvingMutex);
        pthread_cond_wait(&elfSolvingCond, &elfSolvingMutex);
        pthread_mutex_unlock(&elfSolvingMutex);

    }
    return NULL;
}





int main(){
    pthread_t santaThread;
    pthread_t reindeerThreads[NO_REINDEER];
    pthread_t elfThreads[NO_ELFS];

    pthread_create(&santaThread, NULL, santaFunction, NULL);

    for(int i = 0; i < NO_REINDEER; i++){
        pthread_create(&reindeerThreads[i], NULL, reindeerFunction, NULL);
    }

    for(int i = 0; i < NO_ELFS; i++){
        pthread_create(&elfThreads[i], NULL, elfFunction, NULL);
    }

    pthread_join(santaThread, NULL);

    return 0;
}