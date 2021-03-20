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

char *readFile(const char *fileAddress){
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

char *readFileLib(const char *fileAddress){
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

int numberOfOccurences(char* text, const char* word){
    char* pointer = text;
    int result = 0;
    while((pointer = strstr(pointer, word)) != NULL){
        result++;
        pointer += 1;
        
    }
    return result;
}

void solve(const char *inputFile, const char *outputFile, const char *n1String, const char *n2String){
    char *file = readFile(inputFile);
    FILE* output = fopen(outputFile, "w+");

    size_t fileLength = strlen(file), n1Length = strlen(n1String), n2Length = strlen(n2String);
    int numOfOccurences = numberOfOccurences(file, n1String);

    size_t sizeDifference = n2Length - n1Length;
    size_t newSize = fileLength + (numOfOccurences * sizeDifference) - 1;
    

    char* buff = (char*)malloc(newSize * sizeof(char));
    const char* ptr = file;
    int indexNew = 0, indexOld = 0;

    while(indexNew < newSize){
        if(ptr != NULL){
            ptr = strstr(ptr, n1String);
        }
        
        if(ptr != NULL && &file[indexOld] == ptr){
            for(int i = 0; i < n2Length; i++){
                buff[indexNew] = n2String[i];
                indexNew++;
            }
            indexOld += n1Length;
            ptr += n1Length;
        }else{
            buff[indexNew] = file[indexOld];
            indexNew++;
            indexOld++;
        }
    }
    
    buff[newSize] = '\n';
    fwrite(buff, sizeof(char), newSize + 1, output);
    

    fclose(output);
    free(buff);
    free(file);
}

void solveSys(const char *inputFile, const char *outputFile, const char *n1String, const char *n2String){
    char *file = readFileLib(inputFile);
    //FILE* output = fopen(outputFile, "w+");
    int output = open(outputFile, O_WRONLY);

    size_t fileLength = strlen(file), n1Length = strlen(n1String), n2Length = strlen(n2String);
    int numOfOccurences = numberOfOccurences(file, n1String);

    size_t sizeDifference = n2Length - n1Length;
    size_t newSize = fileLength + (numOfOccurences * sizeDifference) - 1;
    

    char* buff = (char*)malloc(newSize * sizeof(char));
    const char* ptr = file;
    int indexNew = 0, indexOld = 0;

    while(indexNew < newSize){
        if(ptr != NULL){
            ptr = strstr(ptr, n1String);
        }
        
        if(ptr != NULL && &file[indexOld] == ptr){
            for(int i = 0; i < n2Length; i++){
                buff[indexNew] = n2String[i];
                indexNew++;
            }
            indexOld += n1Length;
            ptr += n1Length;
        }else{
            buff[indexNew] = file[indexOld];
            indexNew++;
            indexOld++;
        }
    }
    
    buff[newSize] = '\n';
    //fwrite(buff, sizeof(char), newSize + 1, output);
    write(output, buff, newSize + 1);
    

    close(output);
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
    char *intputFile = argv[1];
    char *outputFile = argv[2];
    char *n1String = argv[3];
    char *n2String = argv[4];

    struct tms userTime[3];
    clock_t realTime[3];

    times(&userTime[0]);
    realTime[0] = clock();

    solve(intputFile, outputFile, n1String, n2String);

    times(&userTime[1]);
    realTime[1] = clock();

    solveSys(intputFile, outputFile, n1String, n2String);

    times(&userTime[2]);
    realTime[2] = clock();

    printf("---- Library ----\n");
    calculateAllTimes(userTime, realTime, 1);

    printf("---- System ----\n");
    calculateAllTimes(userTime, realTime, 2);

    return 0;
}