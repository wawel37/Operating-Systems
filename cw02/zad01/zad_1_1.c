#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <dlfcn.h>



char *readFile(char *fileAddress){
    size_t length = 0, fileLength = 0;
    FILE* file = fopen(fileAddress, "r");
    char *buff = NULL;
    
    while(fileLength == length){
        length += 100;
        free(buff);
        buff = (char*)malloc(sizeof(char)* length + 1);
        fileLength = fread(buff, sizeof(char), length, file);
        buff[fileLength] = '\0';
        rewind(file);
    }

    char *result = (char*)malloc(sizeof(char)*fileLength + 1);
    strcpy(result, buff);
    result[fileLength] = '\0';
    free(buff);
    fclose(file);
    return result;
}

char *readFileLib(char *fileAddress){
    size_t length = 0, fileLength = 0;
    int file = open(fileAddress, O_RDONLY);
    char *buff = NULL;
    
    while(fileLength == length){
        length += 100;
        free(buff);
        buff = (char*)malloc(sizeof(char)* length + 1);
        fileLength = read(file, buff, length);
        buff[fileLength] = '\0';
        lseek(file, 0, SEEK_SET);
    }

    char *result = (char*)malloc(sizeof(char)*fileLength + 1);
    strcpy(result, buff);
    result[fileLength] = '\0';
    free(buff);
    close(file);
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

void printMergedFilesLib(char *file1Address, char *file2Address){

    if(file1Address == NULL || file2Address == NULL){
        printf("FILE DOESNT EXISTS!\n");
        return;
    }

    char *buff1 = readFileLib(file1Address);
    char *buff2 = readFileLib(file2Address);

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



double calculateRealTime(clock_t *realTime, int idx){
    return (double) (realTime[idx] - realTime[idx - 1]) / CLOCKS_PER_SEC;
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

int main(int argc, char** argv){
    char *file1Address = argv[1];
    char *file2Address = argv[2];
    struct tms userTime[3];
    clock_t realTime[3];

    times(&userTime[0]);
    realTime[0] = clock();


    printMergedFiles(file1Address, file2Address);

    times(&userTime[1]);
    realTime[1] = clock();

    printMergedFilesLib(file1Address, file2Address);

    times(&userTime[2]);
    realTime[2] = clock();

    printf("---- Library ----\n");
    calculateAllTimes(userTime, realTime, 1);

    printf("---- System ----\n");
    calculateAllTimes(userTime, realTime, 2);

    
    return 0;
}