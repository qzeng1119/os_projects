#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "cpu.h"
#include "schedulers.h"

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

void schedule()
{
    struct node *current = task_list;
    Task *tasks[1024];
    int count = 0;
    int i;

    /*
     * list.c uses head insertion, so the linked list stores tasks in reverse
     * input order. Collect them first, then run them backwards to preserve
     * FCFS behavior.
     */
    while (current != NULL && count < 1024) {
        tasks[count++] = current->task;
        current = current->next;
    }

    for (i = count - 1; i >= 0; i--) {
        run(tasks[i], tasks[i]->burst);
    }
}
