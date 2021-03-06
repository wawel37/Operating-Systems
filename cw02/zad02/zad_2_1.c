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

char *readLine(char *fileContainer, int numOfLine){
    size_t ptr = 0;
    int counter = 0;
    while(counter != numOfLine){
        if(fileContainer[ptr] == '\n'){
            counter++;
        }else if(fileContainer[ptr] == '\0'){
            return NULL;
        }
        ptr++;
    }
    if(fileContainer[ptr] == '\0') return NULL;

    size_t ptr2 = ptr;
    while(fileContainer[ptr2] != '\n'){
        ptr2++;
    }

    int lineLength = ptr2 - ptr + 1;
    char *result = (char*)malloc(sizeof(char)*lineLength + 1);
    for(int i = 0; i < lineLength; i++){
        result[i] = fileContainer[ptr];
        ptr++;
    }
    result[lineLength] = '\0';
    return result;
}

void printLinesContainingSign(char *fileAddress, char sign){
    if(fileAddress == NULL){
        return;
    }
    int counter = 0;
    char *buff = readFile(fileAddress);
    char *line;
    while((line = readLine(buff, counter)) != NULL){
        size_t ptr = 0;
        int flag = 0;
        while(line[ptr] != '\0'){
            if(line[ptr] == sign){
                flag = 1;
            }
            ptr++;
        }

        if(flag == 1){
            printf("%s", line);
        }
        free(line);
        counter++;
    }

    free(buff);
}

void printLinesContainingSignSys(char *fileAddress, char sign){
    if(fileAddress == NULL){
        return;
    }
    int counter = 0;
    char *buff = readFileLib(fileAddress);
    char *line;
    while((line = readLine(buff, counter)) != NULL){
        size_t ptr = 0;
        int flag = 0;
        while(line[ptr] != '\0'){
            if(line[ptr] == sign){
                flag = 1;
            }
            ptr++;
        }

        if(flag == 1){
            printf("%s", line);
        }
        free(line);
        counter++;
    }

    free(buff);
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
    char* fileAddress = argv[2];
    char sign = argv[1][0];

    struct tms userTime[3];
    clock_t realTime[3];

    times(&userTime[0]);
    realTime[0] = clock();

    printLinesContainingSign(fileAddress, sign);

    times(&userTime[1]);
    realTime[1] = clock();

    printLinesContainingSignSys(fileAddress, sign);

    times(&userTime[2]);
    realTime[2] = clock();

    printf("---- Library ----\n");
    calculateAllTimes(userTime, realTime, 1);

    printf("---- System ----\n");
    calculateAllTimes(userTime, realTime, 2);

    return 0;
}