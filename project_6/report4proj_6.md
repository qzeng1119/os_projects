# Report for Project 6

本次项目实现了银行家算法，报告按核心功能模块依次说明。

## 1. 安全性检测（`safe`）

安全性检测是银行家算法的核心。函数维护 `work`（初始为 `available`）和 `finished` 数组，反复扫描所有未完成的进程，找到一个 `need` 不超过 `work` 的进程后，将其 `allocation` 归还给 `work` 并标记为完成：

```c
if (leq(need[i], work) && !finished[i]) break;
// ...
for (int r = 0; r < NUMBER_OF_RESOURCES; r++)
    work[r] += allocation[i][r];
finished[i] = true;
```

若某轮扫描找不到任何可推进的进程（`i == NUMBER_OF_CUSTOMERS`）则退出循环。最终检查 `finished` 数组是否全为 `true`，全为则处于安全状态，否则不安全。

## 2. 资源请求（`request_resources`）

先试探性地修改三个全局数组，再调用 `safe` 检验新状态是否安全：

```c
available[i] -= request[i];
need[customer][i] -= request[i];
allocation[customer][i] += request[i];

if (safe() && leq(allocation[customer], maximum[customer]))
    return 0;

// 不安全则回滚
available[i] += request[i];
need[customer][i] += request[i];
allocation[customer][i] -= request[i];
return -1;
```

"先分配、后检验、不安全则回滚"是银行家算法的标准流程，避免了直接拒绝合法请求的误判。

## 3. 资源释放（`release_resources`）

释放逻辑是请求的逆操作：将释放量从 `allocation` 中扣除，归还给 `available`，并相应增加 `need`，使系统状态恢复一致：

```c
allocation[customer][i] -= release[i];
available[i] += release[i];
need[customer][i] += release[i];
```

## 4. 初始化与交互

程序启动时从命令行读取 `available`，从 `input.txt` 读取 `maximum` 矩阵，并将 `allocation` 初始化为 0 、`need` 初始化为 `maximum`。主循环通过 `fgets` 读取命令，解析后分发到对应函数：`*` 调用 `display` 打印四个数组，`RQ` 调用 `request_resources`，`RL` 调用 `release_resources`，`exit`/`quit` 退出程序。