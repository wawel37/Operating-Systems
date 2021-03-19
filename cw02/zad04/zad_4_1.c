#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *readFile(const char *fileAddress){
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

int numberOfOccurences(const char* text, const char* word){
    size_t len = strlen(word);
    char* pointer;
    int result = 0;
    while((pointer = strstr(text, word)) != NULL){
        result++;
        pointer += sizeof(char)*len;
    }
    return result;
}

void solve(const char *inputFile, const char *outputFile, const char *n1String, const char *n2String){
    const char *file = readFile(inputFile);
    size_t fileLength = strlen(file), ptr = 0, n1Length = strlen(n1String), n2Length = strlen(n2String);
    int numOfOccurences = numberOfOccurences(file, n1String);
    size_t sizeDifference = n2Length - n1Length;

    

    while(file[ptr] )


    free(file);
}


int main(int argc, char *argv){
    char *intputFile = argv[1];
    char *outputFile = argv[2];
    char *n1String = argv[3];
    char *n2String = argv[4];

    return 0;
}