# Report for Project 4

报告分为两个部分：五种调度算法的核心实现，以及两个 Bonus 问题的回答。

## 1. Scheduling Algorithms

前四种算法共用相同的 `add` 函数，（*Prioriy-RR* 由于要根据优先级将线程放入不同的链表，因此 `add` 函数与前四者略有不同），通过课程框架提供的 `insert` 将任务以头插法存入链表，再在 `schedule` 中实现各自的调度逻辑。基础的任务创建与 `run` 调用不再赘述。

**FCFS**：由于 `insert` 为头插，链表中任务顺序与输入顺序相反。因此先将链表收集到数组，再**倒序**遍历调用 `run`，还原先进先出语义。

**SJF**：每轮线性扫描链表，找到 `burst` 最小的任务，将其收集到数组并从链表删除，扫描完毕后按顺序执行，实现非抢占式最短作业优先。

**Priority**：与 SJF 结构相同，每轮扫描改为选取 `priority` 最大的任务，直接 `run` 后删除，无需额外数组。

**RR**：单链表存储所有任务，每轮完整遍历一次链表。若当前任务剩余 `burst ≤ QUANTUM`，执行完毕后删除；否则执行一个时间片并将 `burst` 减去 `QUANTUM`，继续遍历下一个节点，直到链表为空。

**Priority-RR**：用 `task_list[MAX_PRIORITY - MIN_PRIORITY + 1]` 数组为每个优先级维护一条独立链表，`add` 时按 `priority - MIN_PRIORITY` 为索引插入对应链表：
```c
insert(&task_list[priority - MIN_PRIORITY], new_task);
```

`schedule` 从最高优先级向下遍历，对每条非空链表执行 RR 调度（逻辑与纯 RR 相同），高优先级链表全部清空后才进入下一优先级，实现优先级抢占与同级时间片轮转的结合。

## 2. Bonus

### Bonus 1: Race Condition on `tid` & Fix with Atomic Integer

在 SMP 环境下，多个 CPU 各自运行调度器，`add` 函数可能被并发调用。当前实现中 `next_tid` 为普通 `int`，`next_tid++` 在底层是**读—加—写**三步操作，并非原子的。若两个线程同时读到相同的 `next_tid` 值，就会给不同任务分配相同的 `tid`，产生竞态条件。

使用 C11 原子类型可消除此问题：
```c
#include <stdatomic.h>
static atomic_int next_tid = 1;
// 在 add() 中：
new_task->tid = atomic_fetch_add(&next_tid, 1);
```

`atomic_fetch_add` 保证读取与自增作为一个不可分割的原子操作执行，无论多少个 CPU 并发调用 `add`，每个任务都能获得唯一的 `tid`，竞态条件被彻底消除。

### Bonus 2: Performance Metrics

| Algorithm   | Avg Turnaround Time | Avg Waiting Time | Avg Response Time |
|:-----------:|:-------------------:|:---------------:|:-----------------:|
| FCFS        | 94.375              | 73.125          | 73.125            |
| SJF         | 82.5                | 61.25           | 61.25             |
| RR          | 128.125             | 106.875         | 35                |
| Priority    | 96.25               | 75              | 75                |
| Priority-RR | 105                 | 83.75           | 68.75             |

> **Note**: 以 `schedule.txt` 中的数据为计算基准；计算过程中，当调度依据的指标（burst / priority）相同时，按任务名中的数字从小到大优先选择。