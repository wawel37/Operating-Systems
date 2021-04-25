#define  _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define END 0
#define BEG 1


typedef struct Command{
    char **content;
    int length;
}Command;

typedef struct Ingredient{
    Command* commands;
    int length;
    int id;
}Ingredient;

Ingredient* getIngredientWithID(Ingredient *ingredients, int id, int numOfIngredients){
    for(int i = 0; i < numOfIngredients; i++){
        //printf("%d\n", ingredients[i].id);
        if(ingredients[i].id == id){
            //printf("%d\n", ingredients[i].id);
            return &ingredients[i];
        }
    }
    return NULL;
}

void printContent(Ingredient* ingredients, int numOfIngredients){
    for(int i = 0; i < numOfIngredients; i++){
        printf("ingredient id: %d\n", ingredients[i].id);
        printf("ingredient length: %d\n", ingredients[i].length);
        for(int j = 0; j < ingredients[i].length; j++){
            for(int k = 0; k < ingredients[i].commands[j].length; k++){
                printf("%s ", ingredients[i].commands[j].content[k]);
            }
            printf("\n");
        }
    }
}

void freeAllIngredients(Ingredient* ingredients, int numOfIngredients){
    for(int i = 0; i < numOfIngredients; i++){
        for(int j = 0; j < ingredients[i].length; j++){
            for(int k = 0; k < ingredients[i].commands[j].length; k++){
                free(ingredients[i].commands[j].content[k]);
            }
            free(ingredients[i].commands[j].content);
        }
        free(ingredients[i].commands);
    }
}

int getIngredientID(char* buff){
    int result = 0;
    for(char* i = buff + sizeof(char)*8; *i != ' ' && *i != '\n'; i += sizeof(char)){
        //printf("moje i: '%s'\n", i);
        result *= 10;
        result += *i - '0';
    }
    //printf("buffor: %s\n", buff);
    //printf("result: %d\n", result);
    return result;
}

void setIngredientID(char* buff, Ingredient* ingredient){
    //Skladnik
    int result = getIngredientID(buff);
    ingredient->id = result;
}

void processIngredient(char* buff, Ingredient* ingredient){
    int commandsCounter = 1;
    for(char* a = strchr(buff, '|'); a != NULL; a = strchr(a + 1,'|')){
        commandsCounter++;
    }
    
    setIngredientID(buff, ingredient);
    
    ingredient->length = commandsCounter;
    ingredient->commands = (Command*)malloc(sizeof(Command)*commandsCounter);

    char separator[] = " ";
    char* temp;
    char *ptr = strchr(buff, '=') + sizeof(char)*2;
    for(int i = 0; i < commandsCounter && ptr != NULL; i++){
        temp = ptr;
        ptr = strchr(ptr + 1, '|') + sizeof(char)*2;
        temp = strtok(temp, separator);
        int wordCounter = 0;
        while(temp != NULL && strcmp(temp, "|") != 0){
            wordCounter++;
            ingredient->commands[i].content = realloc(ingredient->commands[i].content, wordCounter*sizeof(char*));
            ingredient->commands[i].content[wordCounter - 1] = malloc((strlen(temp) + 1)*sizeof(char));
            strcpy(ingredient->commands[i].content[wordCounter - 1], temp);
            ingredient->commands[i].length = wordCounter;
            temp = strtok(NULL, separator);
        }
    }  
}

void execCommands(char* buff, Ingredient* ingredients, int numOfingredients){
    int ingredientsCounter = 1;
    for(char* a = strchr(buff, '|'); a != NULL; a = strchr(a + 1,'|')){
        ingredientsCounter++;
    }
    int ingredientsID[ingredientsCounter];  

    ingredientsID[0] = getIngredientID(buff);
    int counter = 1;
    for(char* a = strchr(buff, '|'); a != NULL; a = strchr(a + 1,'|')){
        a += sizeof(char) * 2;
        ingredientsID[counter] = getIngredientID(a);
        counter++;
    }

    int commandCounter = 0;
    for(int i = 0; i < counter; i++){
        printf("igid: %d\n", ingredientsID[i]);
        Ingredient* ingredient = getIngredientWithID(ingredients, ingredientsID[i], numOfingredients);
        
        commandCounter += ingredient->length;
    }

    int **fds = malloc(sizeof (int *) * commandCounter);

    for(int i = 0; i < commandCounter; i++) {
        fds[i] = malloc(sizeof (int) * 2);
        pipe(fds[i]);
    }
    int iterator = 0;
    for(int i = 0; i < counter; i++){
        Ingredient* ingredient = getIngredientWithID(ingredients, ingredientsID[i], numOfingredients);

        //Deleting end line char
        int length = strlen(ingredient->commands[ingredient->length - 1].content[ingredient->commands[ingredient->length - 1].length - 1]);
        ingredient->commands[ingredient->length - 1].content[ingredient->commands[ingredient->length - 1].length - 1][length - 1] = '\0';
        
        for(int j = 0; j < ingredient->length; j++){
            if(fork() == 0){
                if(iterator > 0){
                    dup2(fds[iterator - 1][END], STDIN_FILENO);
                }
                if(iterator < commandCounter - 1){
                    dup2(fds[iterator][BEG], STDOUT_FILENO);
                }
                for(int k = 0; k < commandCounter; k++){
                    close(fds[k][BEG]);
                    close(fds[k][END]);
                }
                      
                execvp(ingredient->commands[j].content[0], ingredient->commands[j].content);
            }
            iterator++;
        }
    }
    for(int i = 0; i < commandCounter; i++) {
        close(fds[i][BEG]);
        close(fds[i][END]);
    }
    for(int i = 0; i < commandCounter; i++){
        wait(NULL);
    }

    for(int i = 0; i < commandCounter; i++){
        free(fds[i]);
    }
    free(fds);
}

void processFile(const char* fileName){
    FILE *file = fopen(fileName, "r");
    Ingredient* ingredients = NULL;
    
    if(file == NULL){
        printf("File doesn't exists\n");
        exit(1);
    }

    char *buff = NULL;
    size_t numOfRead, buffLen = 0, numOfIngredients = 0;
    while((numOfRead = getline(&buff, &buffLen, file) != -1)){
        if(numOfRead == 0 || buff[0] == '#'){
            continue;
        }
        
        char *equalsChar = strchr(buff, '=');
        if(equalsChar != NULL){
            numOfIngredients++;
            ingredients = (Ingredient*)realloc(ingredients, sizeof(Ingredient) * numOfIngredients);
            processIngredient(buff, &ingredients[numOfIngredients - 1]);
        }
        else{
            execCommands(buff, ingredients, numOfIngredients);
        }
    }

    fclose(file);
    //printContent(ingredients, numOfIngredients);
    freeAllIngredients(ingredients, numOfIngredients);
}

int main(int argc, char** argv){
    processFile(argv[1]);

    return 0;
}