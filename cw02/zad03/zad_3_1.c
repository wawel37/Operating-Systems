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

int checkIfRoot(int n){
    int flag = 0;
    for(int i = 1; i*i <= n; i++){
        if(i*i == n){
            flag = 1;
        }
    }
    return flag;
}

void solveSys(const char* fileAddress){
    if(fileAddress == NULL){
        printf("There is no valid file name!\n");
        return;
    }
    // FILE *fileA = fopen("a.txt", "w+");
    // FILE *fileB = fopen("b.txt", "w+");
    // FILE *fileC = fopen("c.txt", "w+");

    int fileA = open("a.txt", O_WRONLY);
    int fileB = open("b.txt", O_WRONLY);
    int fileC = open("c.txt", O_WRONLY);


    char *file = readFileLib(fileAddress), *line;
    int counter = 0, evenCounter = 0;
    while((line = readLine(file,counter)) != NULL){
        size_t ptr = 0;
        size_t len = strlen(line);
        if(len > 1){
            while(line[ptr] != '\n'){
                ptr++;
            }
            if((line[ptr - 1] - 48) % 2 == 0){
                evenCounter++;
            }
            if(ptr > 1 && (line[ptr - 2] == '7' || line[ptr - 2] == '0')){
                // fwrite(line, sizeof(char), len, fileB);
                write(fileB, line, len);
            }
            int number = atoi(line);
            if(checkIfRoot(number) == 1){
                //fwrite(line, sizeof(char), len, fileC);
                write(fileC, line, len);
            }
        }
        
        counter++;
        free(line);
    }
    char charNumber[10];
    sprintf(charNumber, "%d", evenCounter);
    char result[30] = "Liczb parzystych jest ";
    strcat(result, charNumber);
    //fwrite(result, sizeof(char), strlen(result), fileA);
    write(fileA, result, strlen(result));


    close(fileA);
    close(fileB);
    close(fileC);
    free(file);
}

void solve(const char* fileAddress){
    if(fileAddress == NULL){
        printf("There is no valid file name!\n");
        return;
    }
    FILE *fileA = fopen("a.txt", "w+");
    FILE *fileB = fopen("b.txt", "w+");
    FILE *fileC = fopen("c.txt", "w+");

    char *file = readFile(fileAddress), *line;
    int counter = 0, evenCounter = 0;
    while((line = readLine(file,counter)) != NULL){
        size_t ptr = 0;
        size_t len = strlen(line);
        if(len > 1){
            while(line[ptr] != '\n'){
                ptr++;
            }
            if((line[ptr - 1] - 48) % 2 == 0){
                evenCounter++;
            }
            if(ptr > 1 && (line[ptr - 2] == '7' || line[ptr - 2] == '0')){
                fwrite(line, sizeof(char), len, fileB);
            }
            int number = atoi(line);
            if(checkIfRoot(number) == 1){
                fwrite(line, sizeof(char), len, fileC);
            }
        }
        
        counter++;
        free(line);
    }
    char charNumber[10];
    sprintf(charNumber, "%d", evenCounter);
    char result[30] = "Liczb parzystych jest ";
    strcat(result, charNumber);
    fwrite(result, sizeof(char), strlen(result), fileA);


    fclose(fileA);
    fclose(fileB);
    fclose(fileC);
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

int main(){
    struct tms userTime[3];
    clock_t realTime[3];

    times(&userTime[0]);
    realTime[0] = clock();

    //solve("dane.txt");

    times(&userTime[1]);
    realTime[1] = clock();

    solveSys("dane.txt");

    times(&userTime[2]);
    realTime[2] = clock();

    printf("---- Library ----\n");
    calculateAllTimes(userTime, realTime, 1);

    printf("---- System ----\n");
    calculateAllTimes(userTime, realTime, 2);

    return 0;
}