#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>

#define CREATE_TABLE "create_table"
#define MERGE_FILES "merge_files"
#define REMOVE_BLOCK_INDEX "remove_block"
#define REMOVE_ROW "remove_row"
#define TEST "test"
#define REMOVE_AND_ADD_BLOCKS 1000

void *dynamicLibHandler;

BlockArray (*dlCreateBlockArray)();
PairArray (*dlCreatePairArray)();
void (*dlMergePairArray)();
int (*dlAddBlock)();
int (*dlGetNumOfLineOfFile)();
int (*dlFindFreeBlockInArray)();
int (*dlGetNumberOfLines)();
void (*dlPrintFile)();
void (*dlFreeLineWithIndex)();
void (*dlFreeBlockWithIndex)();
void (*dlPrintAllBlocks)();
void (*dlFreeAllBlocks)();
void (*dlFreeAllPairs)();

int searchIndexOfArg(const char* text, const char **argv, int argc);

int *searchMultipleIndexesOfArg(const char* text, const char **argv, int argc);

size_t numberOfOccurrencesInArray(const char **argv, int argc, const char *text);

void fillPairArray(PairArray *pairArray, int argc, const char **argv, int mergeFilesIndex);

void printPairArray(PairArray *pairArray);

void printBlockArray(BlockArray *blockArray);

void addAllBlocks(BlockArray *blockArray, PairArray *pairArray);

void removeRows(BlockArray *blockArray, int *indexArray, const char **argv, int argc);

void removeBlockWithIndexes(BlockArray *blockArray, int *indexArray, const char **argv, int argc);

char *generateRandomLine();

void generateFile(char* fileName, int numOfRows);

void generateAllTestFiles(PairArray *pairArray, int numOfRows);

void runTest(int numOfFiles, int numOfRowes);

void runAllTests(const char **argv, int argc, int *testIndexes);

void removeRandomBlocks(BlockArray *blockArray);

void removeAndAddRandomBlock(BlockArray *blockArray, int numOfRows);

double calculateRealTime(clock_t *realTime, int idx);

double calculateUserTime(struct tms *userTime, int idx);

double calculateSystemTime(struct tms *systemTime, int idx);

void calculateAllTimes(struct tms *userTime, clock_t *realTime, int idx);

int main(int argc, const char **argv){

    dynamicLibHandler = dlopen("./lib.so", RTLD_LAZY);

    dlCreateBlockArray = dlsym(dynamicLibHandler, "createBlockArray");
    dlCreatePairArray = dlsym(dynamicLibHandler, "createPairArray");
    dlMergePairArray = dlsym(dynamicLibHandler, "mergePairArray");
    dlAddBlock = dlsym(dynamicLibHandler, "addBlock");
    dlGetNumOfLineOfFile = dlsym(dynamicLibHandler, " getNumOfLineOfFile");
    dlFindFreeBlockInArray = dlsym(dynamicLibHandler, "findFreeBlockInArray");
    dlGetNumberOfLines = dlsym(dynamicLibHandler, "getNumberOfLines");
    dlPrintFile = dlsym(dynamicLibHandler, "printFile");
    dlFreeLineWithIndex = dlsym(dynamicLibHandler, "freeLineWithIndex");
    dlFreeBlockWithIndex = dlsym(dynamicLibHandler, "freeBlockWithIndex");
    dlPrintAllBlocks = dlsym(dynamicLibHandler, "printAllBlocks");
    dlFreeAllBlocks = dlsym(dynamicLibHandler, "freeAllBlocks");
    dlFreeAllPairs = dlsym(dynamicLibHandler, "freeAllPairs");

    if(dynamicLibHandler == NULL){
        printf("No library with that name available\n");
        return 1;
    }




    srand((unsigned) time(NULL));

    int create_tableIndex = searchIndexOfArg(CREATE_TABLE, argv, argc),
        merge_filesIndex = searchIndexOfArg(MERGE_FILES, argv, argc);
    int *remove_blockIndexes = searchMultipleIndexesOfArg(REMOVE_BLOCK_INDEX, argv, argc), 
        *remove_rows = searchMultipleIndexesOfArg(REMOVE_ROW, argv, argc),
        *test = searchMultipleIndexesOfArg(TEST, argv, argc);

    if(numberOfOccurrencesInArray(argv, argc, TEST) > 0){
        runAllTests(argv, argc, test);
    }

    if(create_tableIndex == -1){
        printf("create_table not found!\n");
        return 1;
    }
    if(merge_filesIndex == -1){
        printf("merge_files not found!\n");
        return 1;
    }
    
    BlockArray blockArray;
    PairArray pairArray;
    
    blockArray = dlCreateBlockArray(atoi(argv[create_tableIndex + 1]));
    pairArray = dlCreatePairArray(blockArray.size);
    
    fillPairArray(&pairArray, argc, argv, merge_filesIndex);
    dlMergePairArray(&pairArray);
    addAllBlocks(&blockArray, &pairArray);

    removeRows(&blockArray, remove_rows, argv, argc);
    removeBlockWithIndexes(&blockArray, remove_blockIndexes, argv, argc);
    
    //printBlockArray(&blockArray);
    //printPairArray(&pairArray);



    dlFreeAllBlocks(&blockArray);
    dlFreeAllPairs(&pairArray);
    free(remove_blockIndexes);
    free(remove_rows);
    
    return 0;
}

int searchIndexOfArg(const char* text, const char **argv, int argc){
    for(int i = 0; i < argc; i++){
        if(strcmp(text, argv[i]) == 0){
            return i;
        }
    }
    return -1;
}

int *searchMultipleIndexesOfArg(const char* text, const char **argv, int argc){
    size_t numberOfOccurrences = numberOfOccurrencesInArray(argv, argc, text);

    int *result = calloc(numberOfOccurrences, sizeof(int));
    numberOfOccurrences = 0;
    
    for(int i = 0; i < argc; i++){
        if(strcmp(text, argv[i]) == 0){
            result[numberOfOccurrences++] = i;
        }
    }
    return result;
}

size_t numberOfOccurrencesInArray(const char **argv, int argc, const char *text){
    int result = 0;

    for(int i = 0; i < argc; i++){
        if(strcmp(text, argv[i]) == 0){
            result++;
        }
    }
    return result;
}

void fillPairArray(PairArray *pairArray, int argc, const char **argv, int mergeFilesIndex){
    mergeFilesIndex++;
    for(int i = 0; i < pairArray->size; i++){
        const char* arg = argv[i + mergeFilesIndex];
        const char* first = arg;
        const char* second = strchr(arg, ':') + sizeof(char);
        
        int length1 = (int)(second - first - 1);
        int length2 = strlen(second);

        pairArray->storage[i].fileAddress1 = calloc(length1 + 1, sizeof(char));
        pairArray->storage[i].fileAddress2 = calloc(length2 + 1, sizeof(char));

        // strncpy(pairArray->storage[i].fileAddress1, arg, length1);
        // pairArray->storage[i].fileAddress1[length1] = '\0';
        sscanf(arg, "%[^:]:%s", pairArray->storage[i].fileAddress1, pairArray->storage[i].fileAddress2);
        
        
        // strncpy(pairArray->storage->fileAddress2, second, length2);
        //pairArray->storage[i].fileAddress2[length2] = '\0';
    }
}

void printPairArray(PairArray *pairArray){
    printf("------------------------------ Pair Array ------------------------------\n\n");
    for(int i = 0; i < pairArray->size; i++){
        printf("Pair number %d: \n", i);
        printf("    1. %s\n", pairArray->storage[i].fileAddress1);
        printf("    2. %s\n\n", pairArray->storage[i].fileAddress2);
    }

}

void printBlockArray(BlockArray *blockArray){
    printf("------------------------------ Block Array ----------------------------------------\n\n");
    for(int i = 0; i < blockArray->size; i++){
        printf("Block number %d: \n\n", i);

        for(int j = 0; j < blockArray->storage[i].numOfRows; j++){
            if(blockArray->storage[i].lines[j] != NULL){
                printf("    %d. %s\n", j + 1, blockArray->storage[i].lines[j]);
            }
        }
        printf("\n");
    }
}

void addAllBlocks(BlockArray *blockArray, PairArray *pairArray){
    for(int i = 0; i < pairArray->size; i++){
        dlAddBlock(pairArray->storage[i].tempFile, blockArray);
    }
}

void removeRows(BlockArray *blockArray, int *indexArray, const char **argv, int argc){
    size_t indexArrayLength = numberOfOccurrencesInArray(argv, argc, REMOVE_ROW);
    
    for(int i = 0; i < indexArrayLength; i++){
        int blockIndex = atoi(argv[indexArray[i] + 1]);
        int rowIndex = atoi(argv[indexArray[i] + 2]);

        dlFreeLineWithIndex(&blockArray->storage[blockIndex], rowIndex);
    }
}

void removeBlockWithIndexes(BlockArray *blockArray, int *indexArray, const char **argv, int argc){
    size_t indexArrayLength = numberOfOccurrencesInArray(argv, argc, REMOVE_BLOCK_INDEX);

    for(int i = 0; i < indexArrayLength; i++){
        int blockIndex = atoi(argv[indexArray[i] + 1]);

        dlFreeBlockWithIndex(blockArray, blockIndex);
    }
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

void runTest(int numOfFiles, int numOfRows){
    struct tms userTime[5];
    clock_t realTime[5];


    BlockArray blockArray = dlCreateBlockArray(numOfFiles);
    PairArray pairArray = dlCreatePairArray(numOfFiles);
    

    generateAllTestFiles(&pairArray, numOfRows);

    times(&userTime[0]);
    realTime[0] = clock();

    dlMergePairArray(&pairArray);

    times(&userTime[1]);
    realTime[1] = clock();

    addAllBlocks(&blockArray, &pairArray);

    times(&userTime[2]);
    realTime[2] = clock();

    removeRandomBlocks(&blockArray);

    times(&userTime[3]);
    realTime[3] = clock();

    removeAndAddRandomBlock(&blockArray, numOfRows*2);

    times(&userTime[4]);
    realTime[4] = clock();

    //printBlockArray(&blockArray);
    //printPairArray(&pairArray);

    dlFreeAllBlocks(&blockArray);
    dlFreeAllPairs(&pairArray);

    printf("---- Merge ----\n");
    calculateAllTimes(userTime, realTime, 1);

    printf("---- Save ----\n");

    calculateAllTimes(userTime, realTime, 2);

    printf("---- Delete ----\n");

    calculateAllTimes(userTime, realTime, 3);

    printf("---- Add and Delete ----\n");

    calculateAllTimes(userTime, realTime, 4);

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

void runAllTests(const char **argv, int argc, int *testIndexes){
    size_t testIndexesLength = numberOfOccurrencesInArray(argv, argc, TEST);

    for(int i = 0; i < testIndexesLength; i++){
        int numOfFiles = atoi(argv[testIndexes[i] + 1]);
        int numOfRows = atoi(argv[testIndexes[i] + 2]);
        printf("-------------------------- TEST NOF = %d NOR = %d -------------------------- \n\n", numOfFiles, numOfRows);
        runTest(numOfFiles, numOfRows);
    }
}

void removeRandomBlocks(BlockArray *blockArray){
    for(int i = 0; i < blockArray->size; i++){
        if(rand() % 100 + 1 < 20){
            dlFreeBlockWithIndex(blockArray, i);
        }
    }
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
        dlFreeBlockWithIndex(blockArray, indexToRemove);
        dlAddBlock(file, blockArray);

        fclose(file);
    }


}