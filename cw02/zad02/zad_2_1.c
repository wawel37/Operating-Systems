#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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


    // size_t ptr = 0;
    
    // while(buff[ptr] != '\0'){
    //     char tempBuff[300];
    //     size_t linePtr = 0;
    //     int flag = 0;
        
    //     while(buff[ptr] != '\n'){
    //         if(buff[ptr] == sign){
    //             flag = 1;
    //         }
    //         tempBuff[linePtr] = buff[ptr];
    //         ptr++;
    //         linePtr++;
    //     }
    //     tempBuff[linePtr] = '\0';
    //     ptr++;
    //     if(flag == 1){
    //         printf("%s\n", tempBuff);
    //     }
    // }
    free(buff);
}


int main(int argc, char** argv){
    char* fileAddress = argv[2];
    char sign = argv[1][0];

    printLinesContainingSign(fileAddress, sign);

    return 0;
}