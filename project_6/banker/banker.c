#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

// judge whether the resource array *a* is less or equal to *b*
bool leq(int a[], int b[]) {
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        if (a[i] > b[i]) {
            return false;
        }
    }
    return true;
}

// judge whether the state is safe
bool safe() {
    int work[NUMBER_OF_RESOURCES];
    bool finished[NUMBER_OF_CUSTOMERS] = {false};

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }

    while (true) {
        int i;
        for (i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
            if (leq(need[i], work) && !finished[i]) {
                break;
            }
        }
        if (i == NUMBER_OF_CUSTOMERS) break;

        for (int r = 0; r < NUMBER_OF_RESOURCES; r++) {
            work[r] += allocation[i][r];
        }
        finished[i] = true;
    }

    bool flag = true;
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        if (!finished[i]) {
            flag = false;
            break;
        }
    }

    return flag;
}

int request_resources(int customer, int request[]) {
    if (!leq(request, need[customer]) || !leq(request, available)) {
        return -1;
    }

    // pretend to accept the request
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] -= request[i];
        need[customer][i] -= request[i];
        allocation[customer][i] += request[i];
    }

    // safe and doesn't violate the maximum
    if (safe() && leq(allocation[customer], maximum[customer])) {
        return 0;
    }

    // not safe or already violate the maximum, restore the state
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] += request[i];
        need[customer][i] += request[i];
        allocation[customer][i] -= request[i];
    }
    return -1;
}

// return value: 0: release successfully; -1: the customer doesn't have this much resources
int release_resources(int customer, int release[]) {
    // not enough
    if (!leq(release, allocation[customer])) {
        return -1;
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        allocation[customer][i] -= release[i];
        available[i] += release[i];
        need[customer][i] += release[i];
    }
    return 0;
}

// display the state
void display() {
    // display the available
    printf("available array is:\n");
    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        printf("%d ", available[i]);
    }
    printf("\n");
    // display the maximum
    printf("maximum matrix is:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", maximum[i][j]);
        }
        printf("\n");
    }
    // display the allocation
    printf("allocation matrix is:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", allocation[i][j]);
        }
        printf("\n");
    }
    // display the need
    printf("need matrix is:\n");
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            printf("%d ", need[i][j]);
        }
        printf("\n");
    }
}

static void initialize_state(void) {
    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            allocation[i][j] = 0;
            need[i][j] = maximum[i][j];
        }
    }
}

static int load_maximum_matrix(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return -1;
    }

    for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++) {
        for (int j = 0; j < NUMBER_OF_RESOURCES; j++) {
            if (fscanf(file, " %d", &maximum[i][j]) != 1) {
                fclose(file);
                return -1;
            }
            if (j < NUMBER_OF_RESOURCES - 1) {
                fscanf(file, " ,");
            }
        }
    }

    fclose(file);
    initialize_state();
    return 0;
}

static bool parse_command(const char *line, char command[], int *customer, int values[]) {
    if (sscanf(line, "%15s", command) != 1) {
        return false;
    }

    if (strcmp(command, "*") == 0 || strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
        return true;
    }

    if (strcmp(command, "RQ") == 0 || strcmp(command, "RL") == 0) {
        return sscanf(
            line,
            "%15s %d %d %d %d %d",
            command,
            customer,
            &values[0],
            &values[1],
            &values[2],
            &values[3]
        ) == 6;
    }

    return true;
}

int main(int argc, char *argv[]) {
    const char *input_file = "input.txt";
    char line[256];
    char command[16];
    int customer;
    int values[NUMBER_OF_RESOURCES];

    if (argc != NUMBER_OF_RESOURCES + 1) {
        fprintf(stderr, "Usage: %s r1 r2 r3 r4\n", argv[0]);
        return 1;
    }

    for (int i = 0; i < NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    if (load_maximum_matrix(input_file) != 0) {
        fprintf(stderr, "Failed to read maximum matrix from %s\n", input_file);
        return 1;
    }

    while (true) {
        printf(">");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        if (!parse_command(line, command, &customer, values)) {
            printf("Invalid command format.\n");
            continue;
        }

        if (strcmp(command, "*") == 0) {
            display();
            continue;
        }

        if (strcmp(command, "exit") == 0 || strcmp(command, "quit") == 0) {
            break;
        }

        if (customer < 0 || customer >= NUMBER_OF_CUSTOMERS) {
            printf("Invalid customer number.\n");
            continue;
        }

        if (!leq((int [NUMBER_OF_RESOURCES]){0, 0, 0, 0}, values)) {
            printf("Resources must be non-negative.\n");
            continue;
        }

        if (strcmp(command, "RQ") == 0) {
            if (!leq(values, need[customer])) {
                printf("The customer exceeds its maximum demand.\n");
            } else if (!leq(values, available)) {
                printf("There are not enough available resources.\n");
            } else if (request_resources(customer, values) == 0) {
                printf("Successfully allocate the resources!\n");
            } else {
                printf("The state is not safe!\n");
            }
            continue;
        }

        if (strcmp(command, "RL") == 0) {
            if (release_resources(customer, values) == 0) {
                printf("Successfully release the resources!\n");
            } else {
                printf("%d customer doesn't have this much resources!\n", customer);
            }
            continue;
        }

        printf("Unknown command.\n");
    }
    return 0;
}
