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
    struct node *current = NULL;
    Task *task_del = NULL;
    int min_burst = 0;
    Task *tasks[1024];
    int count = 0;
    
    while (task_list != NULL && count < 1024) {
        current = task_list;
        min_burst = current->task->burst;
        task_del = current->task;
        current = current->next;
        while (current != NULL) {
            if (current->task->burst < min_burst) {
                min_burst = current->task->burst;
                task_del = current->task;
            }
            current = current->next;
        }
        tasks[count++] = task_del;
        delete(&task_list, task_del);
    }
    for (int i = 0; i < count; i++) {
        run(tasks[i], tasks[i]->burst);
    }
}