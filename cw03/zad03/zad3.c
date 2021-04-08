#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define PATH_BUFFOR_SIZE 1024

char *readFile(const char *fileName){
    FILE* file = fopen(fileName, "r");
    fseek(file, 0, SEEK_END);
    size_t fileLength = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *result = (char*)malloc(fileLength*sizeof(char) + 1);
    fread(result, sizeof(char), fileLength, file);
    fclose(file);
    return result;
}



void checkDir(const char *path, const int MAX_DEPTH, const char *PATTERN, int depth){
    //printf("path: %s\n", path);
    DIR* directory = opendir(path);
    if(depth > MAX_DEPTH){
        return;
    }

    if(directory == NULL){
        printf("path that doesnt exits: %s\n", path);
        printf("Dicionary doesnt exist\n");
        exit(0);
    }

    struct dirent *d;
    while((d = readdir(directory)) != NULL){
        //Regular file

        //printf("%d == %d\t fileName = %s\n", DT_REG, d->d_type, d->d_name);
        //printf("%d\n\n", strstr(d->d_name, ".txt") != NULL);

        if(d->d_type == DT_REG && strstr(d->d_name, ".txt") != NULL){
            //printf("came into file\n");

            char newFilePath[PATH_BUFFOR_SIZE];
            if(path[strlen(path) - 1] != '/'){
                snprintf(newFilePath, PATH_BUFFOR_SIZE, "%s/%s", path, d->d_name);
            }
            else{
                snprintf(newFilePath, PATH_BUFFOR_SIZE, "%s%s", path, d->d_name);
            }

            char *file = readFile(newFilePath);

            //printf("file: %s\n", file);

            if(strstr(file, PATTERN) != NULL){
                printf("PID: %d \t PPID: %d \t %s\n", getpid(), getppid(), newFilePath);
            }
            free(file);
        }
        //Directory file
        else if(d->d_type == DT_DIR && strcmp(d->d_name, ".") != 0 && strcmp(d->d_name, "..") != 0){
            
            if(fork() == 0){
                char newDirPath[PATH_BUFFOR_SIZE];

                if(path[strlen(path) - 1] != '/'){
                    snprintf(newDirPath, PATH_BUFFOR_SIZE, "%s/%s", path, d->d_name);
                }else{
                    snprintf(newDirPath, PATH_BUFFOR_SIZE, "%s%s", path, d->d_name);
                }

                closedir(directory);
                checkDir(newDirPath, MAX_DEPTH, PATTERN, depth + 1);
                int status = 0;
                while(wait(&status) > 0);
                exit(0);
            }
        }
    }

    closedir(directory);
}

int main(int argc, char **argv){
    if (argc != 4){
        printf("Wrong number of arguments!\n");
        return 1;
    }
    
    const char *BASE_DIR = argv[1];
    const char *PATTERN = argv[2];
    const int MAX_DEPTH = atoi(argv[3]);

    checkDir(BASE_DIR, MAX_DEPTH, PATTERN, 0);

    int status = 0;
    while(wait(&status) > 0);

    return 0;
}