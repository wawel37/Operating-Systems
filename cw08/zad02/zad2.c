#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

int NUM_OF_THREADS;

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
void *solve2(void *value);
void readPGM(const char *file_name, PGMData *data);
int **allocateMatrix(int n, int m);
void freeImage(int **image, int n);
void printTab(PGMData *image);
void skipComments(FILE *fp);
void writeDataToFile(const char *fileName, PGMData *data);



int main(int argc, char **argv){
    if(argc != 5){
        perror("Wrong number of arguments!");
        return 1;
    }

    NUM_OF_THREADS = atoi(argv[1]);
    const char *splitType = argv[2];
    const char *fileNameIN = argv[3];
    const char *fileNameOUT = argv[4];

    if(strcmp(splitType, "numbers") != 0 && strcmp(splitType, "block") != 0){
        perror("Wrong split type!");
        return 1;
    }
    
    struct timeval start, end;
    gettimeofday(&start, NULL);

    PGMData pgmData;
    pthread_t threads[NUM_OF_THREADS];

    readPGM(fileNameIN, &pgmData);
    int **copy = allocateMatrix(pgmData.row, pgmData.col);

    ThreadData threadDatas[NUM_OF_THREADS];
    for(int i = 0; i < NUM_OF_THREADS; i++){
        threadDatas[i].pgm = &pgmData;
        threadDatas[i].copy = copy;
        threadDatas[i].index = i;
        threadDatas[i].numOfThreads = NUM_OF_THREADS;
        if(strcmp(splitType, "block") == 0){
            pthread_create(&threads[i], NULL, solve, &threadDatas[i]);
        }else if(strcmp(splitType, "numbers") == 0){
            pthread_create(&threads[i], NULL, solve2, &threadDatas[i]);
        }
    }


    freeImage(pgmData.matrix, pgmData.row);
    pgmData.matrix = copy;
    writeDataToFile(fileNameOUT, &pgmData);
    freeImage(pgmData.matrix, pgmData.row);

    gettimeofday(&end, NULL);

    printf("Main thread took %ld μs\n\n", ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec));

    printf("=== %d Threads === \n", NUM_OF_THREADS);
    for(int i = 0; i < NUM_OF_THREADS; i++){
        long *result;
        pthread_join(threads[i], (void *)&result);
        printf("Thread %d: %ld μs\n", i, *result);
    }
    return 0;
}

void *solve(void *value){
    struct timeval start, end;
    gettimeofday(&start, NULL);

    ThreadData *threadData = (ThreadData *) value;
    PGMData *data = threadData->pgm;
    int **copy = threadData->copy;
    int k = threadData->index + 1;

    for(int col = (k-1)*ceil(data->col/NUM_OF_THREADS); col < k*ceil(data->col/NUM_OF_THREADS); col++){
        for(int row = 0; row < data->row; row++){
            copy[row][col] = 255 - data->matrix[row][col];
        }
    }
    
    gettimeofday(&end, NULL);

    long *result = malloc(sizeof(long));
    *result = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);

    pthread_exit((void *)result);
}

void *solve2(void *value){
    struct timeval start, end;
    gettimeofday(&start, NULL);

    ThreadData *threadData = (ThreadData *) value;
    PGMData *data = threadData->pgm;
    int **copy = threadData->copy;

    for(int i = 0; i < data->col*data->row; i++){
        if(i % NUM_OF_THREADS == threadData->index){
            copy[i/data->row][i%data->col] = 255 - data->matrix[i/data->row][i%data->col];
        }
    }

    gettimeofday(&end, NULL);

    long *result = malloc(sizeof(long));
    *result = ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec);

    pthread_exit((void *)result);
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



