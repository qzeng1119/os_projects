# Report for Project 1
报告主要分为三个部分。分别是 *jiffies* 相关代码解释、*seconds* 相关代码解释、对 *bonus* 问题的回答。

## 1. Jiffies
为完成相关任务， 我在 `jiffies_init` 函数（模块入口）和 `jiffies_exit` 函数（模块出口）中分别完成了 *jiffies* 文件的创建和删除。由于这部分代码并不关键，所以不在报告中解释和列出。  

鉴于 `cat` 命令会一直调用 `proc_read` 直到它返回 `0` 这一特性，我采用 `completed` 这个标识变量来保证每次使用 `cat` 命令都会且只会完成一次 `proc_read` 的功能（即打印出 `jiffies` 的值）。

接下来解释代码核心功能的实现。
```c
rv = sprintf(buffer, "jiffies = %lu\n", jiffies);

if (copy_to_user(usr_buf, buffer, rv)) {
    return -EFAULT;
}
```
首先将 `jiffies` 的值按照指定格式写入位于内核内存的 `buffer` 字符数组中，并获得这一字符串的长度 `rv` 。然后调用 `copy_to_user` 函数将字符串从内核内存传递到用户态的 `usr_buf` 中。为了得知内核是否因 `usr_buf` 地址非法而崩溃，采用 `if` 对 `copy_to_user` 函数的返回值进行检查。

## 2. Seconds
这里的代码介绍略过与上面类似的部分，直接解释关键点。

这里我定义了全局变量 `start_time` ，加载模块时，入口函数会把当前的 `jiffies` 值写入此变量。`proc_read` 函数中的核心部分为下面这一行：
```c
rv = sprintf(buffer, "%lu seconds has past since the module loaded.\n", (jiffies - start_time) / HZ);
```
用当前的 `jiffies` 值减去模块加载时记录的 `jiffies` 值就是从模块加载算起硬件中断的总次数，再用它除以硬件中断的频率 `HZ` ， 就能得到从模块加载到本次 `cat` 命令的秒数。

## 3. Bonus: `copy_to_user` 和 `memcpy` 的异同
`copy_to_user` 是 *Linux* 内核中用于将数据从内核空间拷贝到用户空间的安全函数，而 `memcpy` 是标准 C 库中用于用户空间内存拷贝的函数。二者的核心差异在于：  
1. **安全边界检查。** `copy_to_user` 会检查目标用户地址的合法性，防止内核空间访问非法用户内存地址导致系统崩溃或安全漏洞。`memcpy` 不做此类检查，假设所有地址都是有效且可访问的。
2. **处理页缺失。** 当用户空间数据被换出到磁盘时，`copy_to_user` 能够处理这种页缺失异常，并触发页面换入机制；`memcpy` 只能用于内核空间或已驻留内存的用户空间地址，无法处理缺页中断。
3. **使用场景。** `copy_to_user` 专用于内核模块或系统调用中向用户空间返回数据的场景；`memcpy` 适用于用户空间内部或内核空间内部的内存拷贝。