#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 100

typedef struct {
    int beg_idx;
    int end_idx; // point to the one after the last one
} parameters;

int list[MAX_SIZE];
int sorted[MAX_SIZE];

void *sort(void *arg) {
    parameters *p = (parameters *)arg;
    int beg = p -> beg_idx;
    int end = p -> end_idx;
    int flag = 1;
    for (int i = 0; i < end - beg; i++) {
        flag = 1;
        for (int j = beg; j < end - i - 1; j++) {
            if (list[j] > list[j + 1]) {
                int tmp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = tmp;
                flag = 0;
            }
        }
        if (flag) break;
    }
    pthread_exit(NULL);
}

void *merge(void *arg) {
    parameters *p = (parameters *)arg;
    int mid = p -> beg_idx;
    int tail = p -> end_idx;
    int k = 0, i = 0, j = mid;
    while (i < mid && j < tail) {
        while (i < mid && list[i] <= list[j]) sorted[k++] = list[i++];
        while (j < tail && list[j] <= list[i]) sorted[k++] = list[j++];
    }

    while (i < mid) sorted[k++] = list[i++];
    while (j < tail) sorted[k++] = list[j++];

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[3];

    int size = 0;
    while (size < MAX_SIZE) {
        if (scanf("%d", &list[size]) == 1) size++;

        char c = getchar();;
        if (c == '\n' || c == EOF) break;
    }

    int mid = size/2 + 1;
    parameters *data = (parameters *)malloc(3 * sizeof(parameters));
    data[0].beg_idx = 0;
    data[0].end_idx = data[1].beg_idx = mid;
    data[1].end_idx = size;
    pthread_create(&threads[0], NULL, sort, &data[0]);
    pthread_create(&threads[1], NULL, sort, &data[1]);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    data[2].beg_idx = mid;
    data[2].end_idx = size;
    pthread_create(&threads[2], NULL, merge, &data[2]);
    pthread_join(threads[2], NULL);

    for (int i = 0; i < size; i++) {
        printf("%d ", sorted[i]);
    }
    printf("\n");
    free(data);
    return 0;
}