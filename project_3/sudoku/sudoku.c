#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int row;
    int column;
} parameters;

int sudoku[9][9] = {0};
int results[11] = {0};

int is_valid(int num[9]) {
    int check[10] = {0};
    for (int i = 0; i < 9; i++) {
        int val = num[i];
        if (val < 0 || val > 9 || check[val]) return 0;
        check[val] = 1;
    }
    return 1;
}

void *check_rows(void *arg) {
    int row_data[9];
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) row_data[j] = sudoku[i][j];
        if (!is_valid(row_data)) return 0;
    }
    results[0] = 1;
    pthread_exit(NULL);
}

void *check_cols(void *arg) {
    int col_data[9];
    for (int j = 0; j < 9; j++) {
        for (int i = 0; i < 9; i++) col_data[i] = sudoku[i][j];
        if (!is_valid(col_data)) return 0;
    }
    results[1] = 1;
    pthread_exit(NULL);
}

void *check_subgrid(void *arg) {
    parameters *p = (parameters *)arg;
    int start_row = p->row;
    int start_col = p->column;
    int grid_data[9], k = 0;

    for (int i = start_row; i < start_row + 3; i++) {
        for (int j = start_col; j < start_col + 3; j++) {
            grid_data[k++] = sudoku[i][j];
        }
    }
    
    // calculate the index in *results* (2-10)
    int index = 2 + (start_row / 3) * 3 + (start_col / 3);
    if (is_valid(grid_data)) results[index] = 1;

    free(p);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[11];
    int thread_count = 0;

    // get the sudoku
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            scanf("%d", &sudoku[i][j]);
        }
    }

    // check the rows and cols
    pthread_create(&threads[thread_count++], NULL, check_rows, NULL);
    pthread_create(&threads[thread_count++], NULL, check_cols, NULL);

    // check subgrids
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 9; j += 3) {
            parameters *data = (parameters *)malloc(sizeof(parameters));
            data->row = i;
            data->column = j;
            pthread_create(&threads[thread_count++], NULL, check_subgrid, data);
        }
    }

    // wait for all threads
    for (int i = 0; i < 11; i++) {
        pthread_join(threads[i], NULL);
    }

    // check the results
    for (int i = 0; i < 11; i++) {
        if (results[i] == 0) {
            printf("Sudoku is INVALID!\n");
            return 0;
        }
    }
    printf("Sudoku is VALID!\n");
    return 0;
}