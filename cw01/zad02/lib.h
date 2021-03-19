#ifndef _LIB_H
#define _LIB_H
#include <stdio.h>

typedef struct{
    char** lines;
    int numOfRows;
}Block;

typedef struct{
    Block *storage;
    int size;
}BlockArray;

typedef struct{
    char* fileAddress1;
    char* fileAddress2;
    FILE* tempFile;
}Pair;

typedef struct{
    Pair *storage;
    int size;
}PairArray;

void print(char* text);

BlockArray createBlockArray(int numberOfBlocks);

PairArray createPairArray(int numberOfPairs);

void mergePairArray(PairArray *pairArray);

int addBlock(FILE* tempFile, BlockArray *blockArray);

int getNumOfLineOfFile(FILE *filePath);

int findFreeBlockInArray(BlockArray *blockArray);

int getNumberOfLines(Block *block);

void printFile(FILE* file);

void freeLineWithIndex(Block *block, int index);

void freeBlockWithIndex(BlockArray *blockArray, int index);

void printAllBlocks(BlockArray *blockArray);

void freeAllBlocks(BlockArray *blockArray);

void freeAllPairs(PairArray *pairArray);


#endif