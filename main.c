#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int rows1, cols1, rows2, cols2;

struct mat_info {
    int* matrix1;
    int* matrix2;
    int* res;
};

struct row_info {
    int* matrix1;
    int* matrix2;
    int* res;
    long row;
};

struct element_info {
    int* matrix1;
    int* matrix2;
    int* res;
    long row;
    long col;
};


void* mul_by_matrix(void* x) {
    struct mat_info *s = (struct mat_info *)x;
    int i = 0;
    int j;
    int k;
    while (i < rows1) {
        j = 0;
        while (j < cols2) {
            int sum = 0;
            k = 0;
            while (k < cols1) {
                sum += s->matrix1[i * cols1 + k] * s->matrix2[k * cols2 + j];
                k++;
            }
            s->res[i * cols2 + j] = sum;
            j++;
        }
        i++;
    }
    free(s);
}

void* mul_by_row(void* x){
    struct row_info *s = (struct row_info *)x;
    int j = 0;
    while (j < cols2) {
        int sum = 0;
        int k = 0;
        while (k < cols1) {
            sum += s->matrix1[s->row * cols1 + k] * s->matrix2[k * cols2 + j];
            k++;
        }
        s->res[s->row * cols2 + j] = sum;
        j++;
    }
    free(s);
}

void* mul_by_element(void* x){
    struct element_info *s = (struct element_info*)x;
    int k = 0, sum = 0;
    while (k < cols1) {
        sum += s->matrix1[s->row * cols1 + k] * s->matrix2[k * cols2 + s->col];
        k++;
    }
    s->res[s->row * cols2 + s->col] = sum;
    free(s);
}


int main(int argc, char* argv[]) {
    char name1[20];
    char name2[20];
    char name_matrix[20];
    char name_row[20];
    char name_element[20];
    if (argc != 4) {
        argv[1] = "a";
        argv[2] = "b";
        argv[3] = "c";
    }
    strcpy(name1, argv[1]);
    strcpy(name2, argv[2]);
    strcpy(name_matrix, argv[3]);
    strcpy(name_row, argv[3]);
    strcpy(name_element, argv[3]);
    char matrix[16] = "_per_matrix.txt";
    char row[13] = "_per_row.txt";
    char element[17] = "_per_element.txt";
    for (int i = 0; i < 17;i++) {
        if (i < 16) {
            if (i < 13) {
                name_row[strlen(argv[3]) + i] = row[i];
            }
            name_matrix[strlen(argv[3]) + i] = matrix[i];
        }
        name_element[strlen(argv[3]) + i] = element[i];
    }
    int* M1;
    int* M2;
    int* M_per_matrix;
    int* M_per_row;
    int* M_per_element;
    char f[5] = ".txt";
    int i = 0;
    for (i = 0; i < 5; i++) {
        name1[strlen(argv[1]) + i] = f[i];
        name2[strlen(argv[2]) + i] = f[i];
    }
    FILE* mat1 = fopen(name1, "r");
    FILE* mat2 = fopen(name2, "r");
    FILE* mat_matrix = fopen(name_matrix, "w");
    FILE* mat_row = fopen(name_row, "w");
    FILE* mat_element = fopen(name_element, "w");
    fscanf(mat1, "row=%d col=%d", &rows1, &cols1);
    fscanf(mat2, "row=%d col=%d", &rows2, &cols2);
    if (cols1 != rows2) {
        printf("Error\n");
        exit(1);
    }
    M1 = (int*)malloc(sizeof(int) * rows1 *  cols1);
    M2 = (int*)malloc(sizeof(int) * rows2 * cols2);
    M_per_element = (int*)malloc(sizeof(int) * rows1 * cols2);
    M_per_row = (int*)malloc(sizeof(int) * rows1 * cols2);
    M_per_matrix = (int*)malloc(sizeof(int) * rows1 * cols2);
    int k;
    for (i = 0; i < rows2 * cols2; i++) {
        fscanf(mat2, "%d", &M2[i]);
    }
    for (i = 0; i < rows1 * cols1; i++) {
        fscanf(mat1, "%d", &M1[i]);
    }
    fclose(mat1);
    fclose(mat2);
    printf("\n----------------\n");
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols1; j++) {
            printf( "%d ", M1[i * cols1 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    for (int i = 0; i < rows2; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", M2[i * cols2 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    
    //A thread per matrix
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    struct mat_info* info1 = malloc(sizeof(struct mat_info));
    info1->matrix1 = M1;
    info1->matrix2 = M2;
    info1->res = M_per_matrix;
    pthread_t th;
    pthread_create(&th, NULL, &mul_by_matrix, info1);
    pthread_join(th, NULL);
    fprintf(mat_matrix, "Method: A thread per matrix\n");
    fprintf(mat_matrix, "rows:%d col:%d\n", rows1, cols2);
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            fprintf(mat_matrix, "%d ", M_per_matrix[i * cols2 + j]);
        }
        fprintf(mat_matrix, "\n");
    }
    printf("\n----------------\n");
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols1; j++) {
            printf( "%d ", M1[i * cols1 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    for (int i = 0; i < rows2; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", M2[i * cols2 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    free(M_per_matrix);
    fclose(mat_matrix);
    gettimeofday(&stop, NULL);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);

    //A thread per row
    gettimeofday(&start, NULL);
    pthread_t threads[rows1];
    for (long i = 0; i < rows1;i++) {
        struct row_info* info2 = malloc(sizeof(struct row_info));
        info2->matrix1 = M1;
        info2->matrix2 = M2;
        info2->res = M_per_row;
        info2->row = i;
        pthread_create(&threads[i], NULL, &mul_by_row, info2);
    }
    for (i = 0; i < rows1;i++) {
        pthread_join(threads[i], NULL);
    }
    fprintf(mat_row, "Method: A thread per row\n");
    fprintf(mat_row, "rows:%d col:%d\n", rows1, cols2);
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            fprintf(mat_row, "%d ", M_per_row[i * cols2 + j]);
        }
        fprintf(mat_row, "\n");
    }
    printf("\n----------------\n");
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols1; j++) {
            printf( "%d ", M1[i * cols1 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    for (int i = 0; i < rows2; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", M2[i * cols2 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    free(M_per_row);
    fclose(mat_row);
    gettimeofday(&stop, NULL);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    
    //A thread per element
    gettimeofday(&start, NULL); 
    pthread_t threads2[rows1 * cols2];
    for (long i = 0; i < rows1;i++) {
        for (long l = 0; l < cols2;l++) {
            struct element_info* info3 = malloc(sizeof(struct element_info));
            info3->matrix1 = M1;
            info3->matrix2 = M2;
            info3->res = M_per_element;
            info3->row = i;
            info3->col = l;
            pthread_create(&threads2[i * cols2 + l], NULL, &mul_by_element, info3);
        }
    }
    for (i = 0; i < rows1;i++) {
        for (int l = 0; l < cols2;l++) {
            pthread_join(threads2[i * cols2 + l], NULL);
        }
    }
    fprintf(mat_element, "Method: A thread per element\n");
    fprintf(mat_element, "rows:%d col:%d\n", rows1, cols2);
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols2; j++) {
            fprintf(mat_element, "%d ", M_per_element[i * cols2 + j]);
        }
        fprintf(mat_element, "\n");
    }
    printf("\n----------------\n");
    for (i = 0; i < rows1; i++) {
        for (int j = 0; j < cols1; j++) {
            printf( "%d ", M1[i * cols1 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    for (int i = 0; i < rows2; i++) {
        for (int j = 0; j < cols2; j++) {
            printf("%d ", M2[i * cols2 + j]);
        }
        printf("\n");
    }
    printf("\n----------------\n");
    free(M_per_element);
    fclose(mat_element);
    gettimeofday(&stop, NULL);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    free(M1);
    free(M2);
    return 0;
}

