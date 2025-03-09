#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    int** a;
    int** b;
    int** c;
    int aRows;
    int aCols;
    int bRows;
    int bCols;
} Matrices;

typedef struct {
    Matrices* matrices;
    int rowNumber;
} RowMatrix;

typedef struct {
    Matrices* matrices;
    int rowNumber;
    int colNumber;
} ElementMatrix;

int** allocateMatrix(int row, int col) {
    int** matrix = (int**) malloc(row * sizeof(int*));
    if (matrix == NULL) {
        printf("Memory allocation failed for matrix\n");
        exit(1);
    }
    for (int i = 0; i < row; i++) {
        matrix[i] = (int*) malloc(col * sizeof(int));
        if (matrix[i] == NULL) {
            printf("Memory allocation failed for matrix row\n");
            exit(1);
        }
    }
    return matrix;
}

void freeMatrix(int** matrix, int row) {
    for (int i = 0; i < row; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void restResults(Matrices* matrices) {
    for (int i = 0; i < matrices->aRows; i++) {
        for (int j = 0; j < matrices->bCols; j++) {
            matrices->c[i][j] = 0; 
        }
    }
}

void scanMatrix(int** matrix, int rows, int cols, char* fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    int fileRows, fileCols;
    if (fscanf(file, "row=%d col=%d", &fileRows, &fileCols) != 2) {
        printf("Error reading row and col\n");
        fclose(file);
        exit(1);
    }

    if (fileRows != rows || fileCols != cols) {
        printf("Error: Matrix dimensions in file do not match allocated dimensions!\n");
        fclose(file);
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fscanf(file, "%d", &matrix[i][j]) != 1) {
                printf("Error reading matrix values\n");
                fclose(file);
                exit(1);
            }
        }
    }

    fclose(file);
}

void scanMatrices(Matrices* matrices) {
    FILE *fileA = fopen("a.txt", "r");
    FILE *fileB = fopen("b.txt", "r");

    if (!fileA || !fileB) {
        perror("Error opening matrix files");
        exit(1);
    }

    fscanf(fileA, "row=%d col=%d", &matrices->aRows, &matrices->aCols);
    fscanf(fileB, "row=%d col=%d", &matrices->bRows, &matrices->bCols);

    fclose(fileA);
    fclose(fileB);

    matrices->a = allocateMatrix(matrices->aRows, matrices->aCols);
    matrices->b = allocateMatrix(matrices->bRows, matrices->bCols);
    matrices->c = allocateMatrix(matrices->aRows, matrices->bCols); 

    scanMatrix(matrices->a, matrices->aRows, matrices->aCols, "a.txt");
    scanMatrix(matrices->b, matrices->bRows, matrices->bCols, "b.txt");
}

void writeMatrix(int** matrix, int rows, int cols, char* fileName) {
    FILE *file = fopen(fileName, "w"); 
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }

    fprintf(file, "row=%d col=%d\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fprintf(file, "%d\t", matrix[i][j]); 
        }
        fprintf(file, "\n"); 
    }

    fclose(file); 
}

void normalMultiply(Matrices* matrices){
    for (int i = 0; i < matrices->aRows; i++) {     
        for (int j = 0; j < matrices->bCols; j++) { 
            matrices->c[i][j] = 0;
            for (int k = 0; k < matrices->aCols; k++) { 
                matrices->c[i][j] += matrices->a[i][k] * matrices->b[k][j];
            }
        }
    }
}

void* rowThread(void* row){
    RowMatrix* rowMatrix = (RowMatrix*)row;
    for (int i = 0; i < rowMatrix->matrices->bCols; i++) { 
        rowMatrix->matrices->c[rowMatrix->rowNumber][i] = 0;
        for (int j = 0; j < rowMatrix->matrices->aCols; j++) { 
            rowMatrix->matrices->c[rowMatrix->rowNumber][i] += rowMatrix->matrices->a[rowMatrix->rowNumber][j] * rowMatrix->matrices->b[j][i];
        }
    }
    free(rowMatrix);
    return NULL;
}

void rowThreadMultiply(Matrices* matrices){
    pthread_t threads[matrices->aRows];

    for (int i = 0; i < matrices->aRows; i++) {
        RowMatrix* rowMatrices = (RowMatrix*) malloc(sizeof(RowMatrix));
        if (!rowMatrices) {
            printf("Memory allocation failed\n");
            exit(1);
        }
        rowMatrices->matrices = matrices;
        rowMatrices->rowNumber = i;
        if (pthread_create(&threads[i], NULL, rowThread, (void*)rowMatrices)){
            printf("error creating thread.");
            exit(1);
        } 
    }

    for (int i = 0; i < matrices->aRows; i++){
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(1);
        }
    }
}

void* elementThread(void* row){
    ElementMatrix* elementMatrix = (ElementMatrix*)row;
    elementMatrix->matrices->c[elementMatrix->rowNumber][elementMatrix->colNumber] = 0;
    for (int i = 0; i < elementMatrix->matrices->aCols; i++) { 
        elementMatrix->matrices->c[elementMatrix->rowNumber][elementMatrix->colNumber] += elementMatrix->matrices->a[elementMatrix->rowNumber][i] * elementMatrix->matrices->b[i][elementMatrix->colNumber];
    }
    free(elementMatrix);
    return NULL;
}

void elementThreadMultiply(Matrices* matrices){
    pthread_t threads[matrices->aRows * matrices->bCols];

    int k = 0;
    for (int i = 0; i < matrices->aRows; i++) {
        for (int j = 0; j < matrices->bCols; j++) {
            ElementMatrix* elementMatrices = (ElementMatrix*) malloc(sizeof(ElementMatrix));
            if (!elementMatrices) {
                printf("Memory allocation failed\n");
                exit(1);
            }
            elementMatrices->matrices = matrices;
            elementMatrices->rowNumber = i;
            elementMatrices->colNumber = j;
            if (pthread_create(&threads[k++], NULL, elementThread, (void*)elementMatrices)){
                printf("error creating thread.");
                exit(1);
            } 
        }
    }

    for (int i = 0; i < matrices->aRows * matrices->bCols; i++){
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(1);
        }
    }
}

int main(){
    struct timeval stop, start;
    Matrices* matrices = (Matrices*) malloc(sizeof(Matrices));

    scanMatrices(matrices);

    if(matrices->aCols != matrices->bRows){
        printf("Error: These matrices can't be multiplied!\n");
        exit(1);
    }


    gettimeofday(&start, NULL);
    normalMultiply(matrices);
    gettimeofday(&stop, NULL);
    printf("Normal Method take %lu Seconds\n", stop.tv_sec - start.tv_sec);
    printf("Normal Method take %lu microseconds\n\n", stop.tv_usec - start.tv_usec);
    writeMatrix(matrices->c, matrices->aRows, matrices->bCols, "c_per_matrix.txt");
    restResults(matrices);

    gettimeofday(&start, NULL);
    rowThreadMultiply(matrices);
    gettimeofday(&stop, NULL);
    printf("Thread per row take %lu Seconds\n", stop.tv_sec - start.tv_sec);
    printf("Thread per row take %lu microseconds\n\n", stop.tv_usec - start.tv_usec);
    writeMatrix(matrices->c, matrices->aRows, matrices->bCols, "c_per_row.txt");
    restResults(matrices);
    
    gettimeofday(&start, NULL);
    elementThreadMultiply(matrices);
    gettimeofday(&stop, NULL);
    printf("Thread per Element take %lu Seconds\n", stop.tv_sec - start.tv_sec);
    printf("Thread per Element take %lu microseconds\n", stop.tv_usec - start.tv_usec);
    writeMatrix(matrices->c, matrices->aRows, matrices->bCols, "c_per_element.txt");

    freeMatrix(matrices->a, matrices->aRows);
    freeMatrix(matrices->b, matrices->bRows);
    freeMatrix(matrices->c, matrices->aRows);
    free(matrices);
}