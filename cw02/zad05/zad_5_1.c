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

void solveSys(char *file1, char *file2){
    const int N = 5;

    //FILE* output = fopen(file2, "w+");
    int output = open(file2, O_WRONLY);
    char *file = readFile(file1);
    
    size_t additionalSpace = 0, fileLength = strlen(file);
    for(int i = 0; i < fileLength; i++){
        int lineCounter = 0;
        while(file[i] != '\n' && i < fileLength){
            lineCounter++;
            i++;
        }

        additionalSpace += lineCounter / N;
    }

    int newSize = fileLength + additionalSpace + 1;
    char *buff = (char*)malloc(sizeof(char)*(newSize));
    
    int newPtr = 0;
    for(int i = 0; i < fileLength; i++){
        int lineCounter = 0;
        while(file[i] != '\n' && lineCounter != N && i < fileLength){
            lineCounter++;
            i++;
        }
        
        for(int j = i - lineCounter; j < i; j++){
            buff[newPtr] = file[j];
            newPtr++;
        }
        if(lineCounter == N) i--;
        buff[newPtr] = '\n';
        newPtr++;
    }
    
    buff[newSize - 1] = '\n';
    //fwrite(buff, sizeof(char), newSize, output);
    write(output, buff, newSize);
    close(output);
    free(buff);
    free(file);
}

void solve(char *file1, char *file2){
    const int N = 5;

    FILE* output = fopen(file2, "w+");
    char *file = readFile(file1);
    
    size_t additionalSpace = 0, fileLength = strlen(file);
    for(int i = 0; i < fileLength; i++){
        int lineCounter = 0;
        while(file[i] != '\n' && i < fileLength){
            lineCounter++;
            i++;
        }

        additionalSpace += lineCounter / N;
    }

    int newSize = fileLength + additionalSpace + 1;
    char *buff = (char*)malloc(sizeof(char)*(newSize));
    
    int newPtr = 0;
    for(int i = 0; i < fileLength; i++){
        int lineCounter = 0;
        while(file[i] != '\n' && lineCounter != N && i < fileLength){
            lineCounter++;
            i++;
        }
        
        for(int j = i - lineCounter; j < i; j++){
            buff[newPtr] = file[j];
            newPtr++;
        }
        if(lineCounter == N) i--;
        buff[newPtr] = '\n';
        newPtr++;
    }
    
    buff[newSize - 1] = '\n';
    fwrite(buff, sizeof(char), newSize, output);
    fclose(output);
    free(buff);
    free(file);
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

int main(int argc, char **argv){
    char *file1 = argv[1];
    char *file2 = argv[2];

    struct tms userTime[3];
    clock_t realTime[3];

    times(&userTime[0]);
    realTime[0] = clock();

    solve(file1, file2);

    times(&userTime[1]);
    realTime[1] = clock();

    solveSys(file1,file2);

    times(&userTime[2]);
    realTime[2] = clock();

    printf("---- Library ----\n");
    calculateAllTimes(userTime, realTime, 1);

    printf("---- System ----\n");
    calculateAllTimes(userTime, realTime, 2);

    return 0;
}