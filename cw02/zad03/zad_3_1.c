#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *readFile(const char *fileAddress){
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


int main(){

    solve("dane.txt");

    return 0;
}