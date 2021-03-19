#include <stdio.h>
#include "lib.h"



int main(){
    const int numOfPairs = 2;
    const int numOfBlocks = 2;

    PairArray pairs = createPairArray(numOfPairs);
    pairs.storage[0].fileAddress1 = "file_1.txt";
    pairs.storage[0].fileAddress2 = "file_2.txt";
    pairs.storage[1].fileAddress1 = "file_3.txt";
    pairs.storage[1].fileAddress2 = "file_4.txt"; 

    mergePairArray(&pairs);

    BlockArray blocks = createBlockArray(numOfBlocks);
    for(int j = 0; j < blocks.size; j++){
        addBlock(pairs.storage[j].tempFile, &blocks);
    }
    printAllBlocks(&blocks);


    for(int i = 0; i < blocks.size; i++){
        freeBlockWithIndex(&blocks, i);
    }
    fclose(pairs.storage[0].tempFile);
    return 0;
}
