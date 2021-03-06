#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>


const int NUM_OF_THREADS = 6;

typedef struct PGMData{
    int row;
    int col;
    int max_gray;
    int **matrix;
    char version[3];
}PGMData;

typedef struct ThreadData{
    PGMData* pgm;
    int **copy;
    int numOfThreads;
    int index;
}ThreadData;


void *solve(void *value);
void readPGM(const char *file_name, PGMData *data);
int **allocateMatrix(int n, int m);
void freeImage(int **image, int n);
void printTab(PGMData *image);
void skipComments(FILE *fp);
void writeDataToFile(const char *fileName, PGMData *data);
void negate(int **copy, int row, int column); //deprecated
//void negate(PGMData *image); //deprecated
void copyTable(PGMData *dest, int **source);



int main(){
    PGMData pgmData;
    pthread_t threads[NUM_OF_THREADS];

    readPGM("apollonian_gasket.ascii.pgm", &pgmData);
    printf("%d %d \n", pgmData.col, pgmData.row);
    int **copy = allocateMatrix(pgmData.row, pgmData.col);

    ThreadData threadDatas[NUM_OF_THREADS];
    for(int i = 0; i < NUM_OF_THREADS; i++){
        threadDatas[i].pgm = &pgmData;
        threadDatas[i].copy = copy;
        threadDatas[i].index = i;
        threadDatas[i].numOfThreads = NUM_OF_THREADS;
        pthread_create(&threads[i], NULL, solve, &threadDatas[i]);
    }

    for(int i = 0; i < NUM_OF_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    freeImage(pgmData.matrix, pgmData.row);
    pgmData.matrix = copy;
    writeDataToFile("a.ascii.pgm", &pgmData);

    freeImage(pgmData.matrix, pgmData.row);
    return 0;
}

void *solve(void *value){
    ThreadData *threadData = (ThreadData *) value;
    PGMData *data = threadData->pgm;
    int **copy = threadData->copy;

    for(int i = 0; i < data->col*data->row; i++){
        if(i % NUM_OF_THREADS == threadData->index){
            //printf("Coords: %d %d \t index: %d\n", i/data->row, i%data->col, threadData->index);
            copy[i/data->row][i%data->col] = 255 - data->matrix[i/data->row][i%data->col];
            //sleep(1);
        }
    }
    return NULL;
}

int **allocateMatrix(int n, int m){
    int **result = malloc(sizeof(int*)*n);
    for(int i = 0; i < n; i++){
        result[i] = malloc(sizeof(int)*m);
    }
    return result;
}

void freeImage(int **image, int n){
    for(int i = 0; i < n; i++){
        free(image[i]);
    }
    free(image);
}

void printTab(PGMData *image){
    printf("num of cols: %d\n", image->col);
    printf("num of rows: %d\n", image->row);
    printf("grey: %d\n", image->max_gray);
    int **tab = image->matrix;
    for(int i = 0; i < image->col; i++){
        for(int j = 0; j < image->row; j++){
            printf("%d ", tab[i][j]);
        }
        printf("\n");
    }
}

void readPGM(const char *file_name, PGMData *data)
{
    FILE *pgmFile;
    pgmFile = fopen(file_name, "rb");
    fgets(data->version, sizeof(data->version), pgmFile);

    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->row);
    fscanf(pgmFile, "%d", &data->col);

    skipComments(pgmFile);
    fscanf(pgmFile, "%d", &data->max_gray);

    data->matrix = allocateMatrix(data->row, data->col);

    for(int i = 0; i < data->row; i++){
        for(int j = 0; j < data->col; j++){
            fscanf(pgmFile, "%d", &data->matrix[i][j]);
        }
    }
    fclose(pgmFile);
}

void skipComments(FILE *fp){
    int ch;
    char line[100];
    while ((ch = fgetc(fp)) != EOF && isspace(ch));
 
    if (ch == '#') {
        fgets(line, sizeof(line), fp);
        skipComments(fp);
    } else {
        fseek(fp, -1, SEEK_CUR);
    }
} 

void writeDataToFile(const char *fileName, PGMData *data){
    FILE* file = fopen(fileName, "w+");
    fprintf(file, "%s\n", data->version);
    fprintf(file, "%d %d\n", data->row, data->col);
    fprintf(file, "%d\n", data->max_gray);
    for(int i = 0; i < data->row; i++){
        for(int j = 0; j < data->col; j++){
            fprintf(file, "%d ", data->matrix[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// void negate(PGMData *image){
//     for(int i = 0; i < image->row; i++){
//         for(int j = 0; j < image->col; j++){
//             image->matrix[i][j] = 255 - image->matrix[i][j];
//         }
//     }
// }
void negate(int **copy, int row, int column){
    for(int i = 0; i < row; i++){
        for(int j = 0; j < column; j++){
            copy[i][j] = 255 - copy[i][j];
        }
    }
}

void copyTable(PGMData *dest, int **source){
    for(int i = 0; i < dest->row; i++){
        for(int j = 0; j < dest->col; j++){
            dest->matrix[i][j] = source[i][j];
        }
    }
}