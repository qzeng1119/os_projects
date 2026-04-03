#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"
#include "list.h"
#include "cpu.h"

static struct node *task_list = NULL;
static int next_tid = 1;

void add(char *name, int priority, int burst)
{
    Task *new_task = malloc(sizeof(Task));

    new_task->name = strdup(name);
    new_task->tid = next_tid++;
    new_task->priority = priority;
    new_task->burst = burst;

    insert(&task_list, new_task);
}

void schedule() {
    struct node *current = NULL;
    Task *task_del = NULL;

    while (task_list != NULL) {
        current = task_list;
        while (current != NULL) {
            if (current->task->burst <= QUANTUM) {
                run(current->task, current->task->burst);
                task_del = current->task;
                current = current->next;
                delete(&task_list, task_del);
                continue;
            } else {
                run(current->task, QUANTUM);
                current->task->burst -= QUANTUM;
                current = current->next;
            }
        }
    }
}