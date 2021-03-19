#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *readFile(char *fileAddress){
    size_t length = 0, fileLength = 0;
    FILE* file = fopen(fileAddress, "r");
    char *buff = NULL;
    
    while(fileLength == length){
        length += 1000;
        free(buff);
        buff = (char*)malloc(sizeof(char)* length);
        fileLength = fread(buff, sizeof(char), length, file);
    }

    char *result = (char*)malloc(sizeof(char)*fileLength + 1);
    strcpy(result, buff);
    result[fileLength] = '\0';
    free(buff);
    fclose(file);
    return result;
}

void printMergedFiles(char *file1Address, char *file2Address){

    if(file1Address == NULL || file2Address == NULL){
        printf("FILE DOESNT EXISTS!\n");
        return;
    }

    char *buff1 = readFile(file1Address);
    char *buff2 = readFile(file2Address);

    size_t ptr1 = 0, ptr2 = 0;
    //NA KONCU KAZDEGO PLIKU MUSI BYC SPACJA
    while(buff1[ptr1] != '\0' || buff2[ptr2] != '\0'){
        
        if(buff1[ptr1] != '\n' && buff1[ptr1] != '\0'){
            while(buff1[ptr1] != '\n' && buff1[ptr1] != '\0'){
                printf("%c", buff1[ptr1]);
                ptr1++;
            }
            printf("\n");
            ptr1++;
        }

        if(buff2[ptr2] != '\n' && buff2[ptr2] != '\0'){
            while(buff2[ptr2] != '\n' && buff2[ptr2] != '\0'){
                printf("%c", buff2[ptr2]);
                ptr2++;
            }
            printf("\n");
            ptr2++;
        }
    }
    free(buff1);
    free(buff2);
}


int main(int argc, char** argv){
    char *file1Address = argv[1];
    char *file2Address = argv[2];
    

    printMergedFiles(file1Address, file2Address);
    return 0;
}