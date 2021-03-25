#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/wait.h>

#define REMOVE_AND_ADD_BLOCKS 1000

void runTest(int numOfFiles, int numOfRows);

void runAllTests(int numOfFiles, int numOfRows);

void generateAllTestFiles(PairArray *pairArray, int numOfRows);

void addAllBlocks(BlockArray *blockArray, PairArray *pairArray);

void removeRandomBlocks(BlockArray *blockArray);

void removeAndAddRandomBlock(BlockArray *blockArray, int numOfRows);

void calculateAllTimes(struct tms *userTime, clock_t *realTime, int idx);

double calculateSystemTime(struct tms *systemTime, int idx);

double calculateUserTime(struct tms *userTime, int idx);

double calculateRealTime(clock_t *realTime, int idx);

char *generateRandomLine();

void generateFile(char* fileName, int numOfRows);

int main(){
    runAllTests(5,10);
    runAllTests(5,100);
    runAllTests(200,100);
    runAllTests(200,400);
    runAllTests(1000,100);
    runAllTests(1000,600);
}

char *generateRandomLine(){

    const int SIZE = (rand() % 10) + 20;
    const char* CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz";

    const size_t NUMBER_OF_CHARS = strlen(CHARACTERS);

    char* result = (char*)malloc(SIZE + 2 * sizeof(char));

    for(int i = 0; i < SIZE; i++){
        result[i] = CHARACTERS[rand() % NUMBER_OF_CHARS];
    }
    result[SIZE] = '\n';
    result[SIZE + 1] = '\0';

    return result;
}

double calculateRealTime(clock_t *realTime, int idx){
    return (double) (realTime[idx] - realTime[idx - 1]) /CLOCKS_PER_SEC;
}

double calculateUserTime(struct tms *userTime, int idx){
    return (double) (userTime[idx].tms_utime - userTime[idx - 1].tms_utime) / sysconf(_SC_CLK_TCK);
}

double calculateSystemTime(struct tms *systemTime, int idx){
    return (double) (systemTime[idx].tms_stime - systemTime[idx - 1].tms_stime) / sysconf(_SC_CLK_TCK);
}

void calculateAllTimes(struct tms *userTime, clock_t *realTime, int idx){
    printf("REAL: %lf\n", calculateRealTime(realTime, idx));
    printf("USER: %lf\n", calculateUserTime(userTime, idx));
    printf("SYSTEM: %lf\n", calculateSystemTime(userTime, idx));
}

void removeAndAddRandomBlock(BlockArray *blockArray, int numOfRows){
    
    for(int i = 0; i < REMOVE_AND_ADD_BLOCKS; i++){
        char* buff;
        FILE* file = tmpfile();
        for(int i = 0; i < numOfRows; i++){
            buff = generateRandomLine();
            fprintf(file, "%s", buff);
            free(buff);
        }
        int indexToRemove = 0;
        while(blockArray->storage[indexToRemove].lines == NULL){
            indexToRemove = rand() % blockArray->size;
        }
        freeBlockWithIndex(blockArray, indexToRemove);
        addBlock(file, blockArray);

        fclose(file);
    }


}

void removeRandomBlocks(BlockArray *blockArray){
    for(int i = 0; i < blockArray->size; i++){
        if(rand() % 100 + 1 < 20){
            freeBlockWithIndex(blockArray, i);
        }
    }
}

void addAllBlocks(BlockArray *blockArray, PairArray *pairArray){
    for(int i = 0; i < pairArray->size; i++){
        addBlock(pairArray->storage[i].tempFile, blockArray);
    }
}

void runAllTests(int numOfFiles, int numOfRows){

    printf("-------------------------- TEST NOF = %d NOR = %d -------------------------- \n\n", numOfFiles, numOfRows);
    runTest(numOfFiles, numOfRows);
}

void runTest(int numOfFiles, int numOfRows){
    struct tms userTime[5];
    clock_t realTime[5];

    PairArray pairArray = createPairArray(numOfFiles);
    
    
    generateAllTestFiles(&pairArray, numOfRows);

    times(&userTime[0]);
    realTime[0] = clock();

    mergePairArray(&pairArray);

    for(int i = 0; i < pairArray.size; i++){

        if(fork() == 0){
            FILE *fileA = fopen(pairArray.storage[i].fileAddress1, "r");
            FILE *fileB = fopen(pairArray.storage[i].fileAddress2, "r");
            // FILE *mergedFile = fopen(pairArray->storage[i].mergedAddress, "w+");
            pairArray.storage[i].tempFile = tmpfile();

            if(fileA == NULL || fileB == NULL){
                printf("File opening error, one of the files is null!\n");
            }

            size_t buffSize = 0;
            char *buff = NULL;
            int flagA = 1, flagB = 1;

            while(flagA || flagB){

                if(flagA && getline(&buff, &buffSize, fileA) != -1){
                    fprintf(pairArray.storage[i].tempFile, "%s", buff);
                }else{
                    flagA = 0;
                }

                if(flagB && getline(&buff, &buffSize, fileB) != -1){
                    
                    fprintf(pairArray.storage[i].tempFile, "%s", buff);
                }else{
                    flagB = 0;
                }
            }
            
            free(buff);
            fclose(fileA);
            fclose(fileB);
            rewind(pairArray.storage[i].tempFile);
            freeAllPairs(&pairArray);
            exit(0);
        }
    }
    int status = 0;
    while(wait(&status) > 0);

    times(&userTime[1]);
    realTime[1] = clock();

    printf("---- Merge ----\n");
    calculateAllTimes(userTime, realTime, 1);

}

void generateAllTestFiles(PairArray *pairArray, int numOfRows){

    static int numOfTestFileGeneration = 1;
    char generationString[5];
    sprintf(generationString, "%d_", numOfTestFileGeneration);

    for(int i = 0; i < pairArray->size; i++){

        char baseFileName[20] = "test_file_a_";
        strcat(baseFileName, generationString);

        char fileNumber[30];
        sprintf(fileNumber, "%d", i);
        strcat(baseFileName, fileNumber);

        int fileNameLength = strlen(baseFileName);

        pairArray->storage[i].fileAddress1 = (char*)malloc((fileNameLength + 1) * sizeof(char));
        pairArray->storage[i].fileAddress2 = (char*)malloc((fileNameLength + 1) * sizeof(char));

        strcpy(pairArray->storage[i].fileAddress1, baseFileName);
        generateFile(baseFileName, numOfRows);

        baseFileName[10] = 'b';

        strcpy(pairArray->storage[i].fileAddress2, baseFileName);
        generateFile(baseFileName, numOfRows);
    }
    numOfTestFileGeneration++;
}

void generateFile(char* fileName, int numOfRows){
    FILE* file = fopen(fileName, "w+");
    char* buff;
    for(int i = 0; i < numOfRows; i++){
        buff = generateRandomLine();
        fprintf(file, "%s", buff);
        free(buff);
    }
    fclose(file);
}