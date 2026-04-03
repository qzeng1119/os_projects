#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"
#include "task.h"

static struct node *task_list[MAX_PRIORITY - MIN_PRIORITY + 1] = {NULL};
static int next_tid = 1;

void add(char *name, int priority, int burst)
{
    Task *new_task = malloc(sizeof(Task));

    new_task->name = strdup(name);
    new_task->tid = next_tid++;
    new_task->priority = priority;
    new_task->burst = burst;

    insert(&task_list[priority - MIN_PRIORITY], new_task);
}

void schedule() {
    struct node *current = NULL;
    Task *task_del = NULL;

    for (int i = MAX_PRIORITY - MIN_PRIORITY; i >= 0; i--) {
        while (task_list[i] != NULL) {
            current = task_list[i];
            while (current != NULL) {
                if (current->task->burst <= QUANTUM) {
                    run(current->task, current->task->burst);
                    task_del = current->task;
                    current = current->next;
                    delete(&task_list[i], task_del);
                    continue;
                } else {
                    run(current->task, QUANTUM);
                    current->task->burst -= QUANTUM;
                    current = current->next;
                }
            }
        }
    }
}