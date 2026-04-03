#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "cpu.h"
#include "schedulers.h"
#include "task.h"

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
    struct node* current = NULL;
    Task *task_del = NULL;
    int max_pri = MIN_PRIORITY - 1;

    while (task_list != NULL) {
        current = task_list;
        max_pri = MIN_PRIORITY - 1;
        while (current != NULL) {
            if (current->task->priority > max_pri) {
                task_del = current->task;
                max_pri = current->task->priority;
            }
            current = current->next;
        }
        run(task_del, task_del->burst);
        delete(&task_list, task_del);
    }
}