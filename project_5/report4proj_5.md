# Report for Project 5

报告分为三个部分：线程池的核心实现、生产者-消费者问题的核心实现，以及 Bonus 问题的回答。

## 1. Thread Pool

整体思路是维护一个链表任务队列和固定数量的 worker 线程。提交任务时将其加入队尾并通过信号量通知 worker；worker 线程持续阻塞等待信号量，唤醒后取出任务执行。

**任务队列**：`enqueue` 和 `dequeue` 均用 `queue_mutex` 保护对链表的操作，确保多个 worker 并发取任务时不发生竞态。`enqueue` 采用尾插保证 FIFO 顺序。

**任务提交与 worker 协作**：

```c
// pool_submit
status = enqueue(new_task);
sem_post(&task_available);

// worker
sem_wait(&task_available);
next_task = dequeue();
execute(next_task.function, next_task.data);
```

`pool_submit` 将任务入队后调用 `sem_post` 递增信号量；worker 线程在 `sem_wait` 处阻塞，每当信号量大于 0 时被唤醒并取出一个任务执行。信号量的计数天然与队列中的任务数保持一致，无需额外轮询。

**关闭线程池**：`pool_shutdown` 对所有 worker 调用 `pthread_cancel` 强制取消，再用 `pthread_join` 等待它们退出，最后遍历链表释放残余节点并销毁互斥锁和信号量。

## 2. Producer-Consumer

整体思路是用一个循环数组实现有界缓冲区，以 `empty` 和 `full` 两个计数信号量分别追踪空槽和满槽数量，以互斥锁保护对缓冲区下标的并发访问。生产者和消费者线程数由命令行参数指定，主线程睡眠指定时间后通过 `running` 标志和 `pthread_cancel` 终止所有线程。

**插入与取出**：两个操作结构对称，以插入为例：

```c
sem_wait(&empty);           // 等待空槽
pthread_mutex_lock(&buffer_mutex);
buffer[insert_index] = item;
insert_index = (insert_index + 1) % BUFFER_SIZE;  // 循环推进下标
pthread_mutex_unlock(&buffer_mutex);
sem_post(&full);            // 通知消费者
```

先对 `empty` 执行 `sem_wait`：若缓冲区已满则阻塞，否则进入临界区写入数据并循环推进 `insert_index`，退出后对 `full` 执行 `sem_post` 通知消费者。取出操作方向相反，操作 `full` 和 `empty`，并推进 `remove_index`。

**`rand()` 的线程安全**：标准库的 `rand()` 不是线程安全函数，`main.c` 用 `rand_mutex` 将每次调用包裹在临界区内，避免多线程并发调用导致的数据竞争。

## 3. Bonus: 线程池核心线程数的设置

线程数过少时，任务队列积压，CPU 可能处于空闲状态，吞吐量和响应时间均会下降。线程数过多时，线程创建与销毁的开销、上下文切换频率以及锁竞争都会显著增加，反而降低整体性能，在任务本身执行时间较短时尤为明显。

合理设置线程数通常参考以下原则：对于 **CPU 密集型**任务，核心线程数设为逻辑 CPU 核心数（即 `N_cpu`）左右，使每个核心保持满载而不产生多余切换；对于 **I/O 密集型**任务，线程在等待 I/O 时会主动阻塞，可适当增大至 `N_cpu × (1 + 等待时间/计算时间)`，以充分利用 CPU 空闲时间。此外，也可结合实际负载进行基准测试，在吞吐量、延迟和资源占用之间找到经验最优值。