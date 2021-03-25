#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

void print(char *text){
    printf("%s", text);
}

BlockArray createBlockArray(int numberOfBlocks){

    BlockArray result;

    if(numberOfBlocks < 0) numberOfBlocks = 0;

    result.storage = calloc(numberOfBlocks, sizeof(Block));
    result.size = numberOfBlocks;
    return result;
}

PairArray createPairArray(int numberOfPairs){

    PairArray result;

    if (numberOfPairs < 0) numberOfPairs = 0;

    result.storage  = calloc(numberOfPairs, sizeof(Pair));
    result.size = numberOfPairs;
    return result;
}

void mergePairArray(PairArray *pairArray){

    for(int i = 0; i < pairArray->size; i++){
        
        FILE *fileA = fopen(pairArray->storage[i].fileAddress1, "r");
        FILE *fileB = fopen(pairArray->storage[i].fileAddress2, "r");
        // FILE *mergedFile = fopen(pairArray->storage[i].mergedAddress, "w+");
        pairArray->storage[i].tempFile = tmpfile();

        if(fileA == NULL || fileB == NULL){
            printf("File opening error, one of the files is null!\n");
        }

        size_t buffSize = 0;
        char *buff = NULL;
        int flagA = 1, flagB = 1;

        while(flagA || flagB){

            if(flagA && getline(&buff, &buffSize, fileA) != -1){
                fprintf(pairArray->storage[i].tempFile, "%s", buff);
            }else{
                flagA = 0;
            }

            if(flagB && getline(&buff, &buffSize, fileB) != -1){
                
                fprintf(pairArray->storage[i].tempFile, "%s", buff);
            }else{
                flagB = 0;
            }
        }
        
        free(buff);
        fclose(fileA);
        fclose(fileB);
        rewind(pairArray->storage[i].tempFile);
    }
}

int addBlock(FILE* tempFile, BlockArray *blockArray){
        int numberOfLines = getNumOfLineOfFile(tempFile);
        int index = findFreeBlockInArray(blockArray);
        size_t buffSize = 0;

        //If the blockArray is full
        if(index < 0){
            return index;
        }

        blockArray->storage[index].lines = calloc(numberOfLines, sizeof(char *));

        for(int i = 0; i < numberOfLines; i++){
            getline(&blockArray->storage[index].lines[i], &buffSize, tempFile);
            buffSize = 0;
        }

        blockArray->storage[index].numOfRows = numberOfLines;

        rewind(tempFile);
        return index;
}

int getNumOfLineOfFile(FILE *tempFile){
    int result = 0;
    char* buff = NULL;
    size_t buffSize = 0;

    while(getline(&buff, &buffSize, tempFile) != -1){
        result++;
    }
    free(buff);
    rewind(tempFile);
    return result;
}

int findFreeBlockInArray(BlockArray *blockArray){
    for(int i = 0; i < blockArray->size; i++){
        if(blockArray->storage[i].lines == NULL){
            return i;
        }
    }
    return -1;
}

int getNumberOfLines(Block *block){
    return block->numOfRows;
}

void printFile(FILE* file){
    char* buff = NULL;
    size_t buffSize = 0;
    while(getline(&buff, &buffSize, file) != -1){
        printf("%s", buff);
    }

    free(buff);
    rewind(file);
}

void freeBlockWithIndex(BlockArray *blockArray, int index){

    if(index < 0 || index >= blockArray->size){
        printf("Wrong index when freeing block!\n");
        return;
    }

    Block* block = &blockArray->storage[index];

    for(int i = 0; i < block->numOfRows; i++){
        free(block->lines[i]);
        block->lines[i] = NULL;
    }

    free(block->lines);
    blockArray->storage[index].lines = NULL;
    blockArray->storage[index].numOfRows = 0;
}

void freeLineWithIndex(Block *block, int index){

    if(index < 0 || index >= block->numOfRows){
        printf("Worng index when freeing line!\n");
        return;
    }

    free(block->lines[index]);
    block->lines[index] = NULL;
}

void freeAllBlocks(BlockArray *blockArray){
    for(int i = 0; i < blockArray->size; i++){
        freeBlockWithIndex(blockArray, i);
    }
}

void freeAllPairs(PairArray *pairArray){
    for(int i = 0; i < pairArray->size; i++){
        free(pairArray->storage[i].fileAddress1);
        free(pairArray->storage[i].fileAddress2);
        fclose(pairArray->storage[i].tempFile);
    }
   
}

void printAllBlocks(BlockArray *blockArray){
    int lineCounter = 1, blockCounter = 1;
    for(int i = 0; i < blockArray->size; i++){

        printf("Block number: %d\n", blockCounter);

        for(int j = 0; j < blockArray->storage[i].numOfRows; j++){
            printf("%d. %s\n", lineCounter, blockArray->storage[i].lines[j]);
            lineCounter++;
        }
        printf("\n");
        lineCounter = 1;
        blockCounter++;
    }
}
