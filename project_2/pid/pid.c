#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/sched.h>

#define PROC_NAME "pid"
static int last_pid = -1; // kernel space

static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos) {
    char *k_mem;

    // allocate kernel memory
    k_mem = kmalloc(count + 1, GFP_KERNEL);
    
    if (copy_from_user(k_mem, usr_buf, count)) {
        kfree(k_mem);
        return -EFAULT;
    }
    k_mem[count] = '\0'; // make sure the string ends with null

    if (kstrtoint(k_mem, 10, &last_pid) != 0) { // check the return value
        printk(KERN_INFO "Error converting string to int\n");
    }

    kfree(k_mem); // release the memory
    return count;
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    char buffer[128];
    int rv = 0;
    struct task_struct *task;
    struct pid *pid_struct;

    // according to the feature of the "cat" command
    if (*pos > 0 || last_pid == -1) return 0;

    // get pid_struct
    pid_struct = find_vpid(last_pid);
    if (!pid_struct) {
        printk(KERN_INFO "Invalid PID: %d\n", last_pid);
        return 0;
    }

    // get task_struct
    task = pid_task(pid_struct, PIDTYPE_PID);
    if (!task) return 0;

    // get the information needed
    rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%u]\n", task->comm, task->pid, task->__state);

    if (copy_to_user(usr_buf, buffer, rv)) return -EFAULT;

    *pos += rv;
    return rv;
}

static const struct proc_ops pid_ops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init pid_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &pid_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit pid_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}
module_init(pid_init);
module_exit(pid_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Pid Module");
MODULE_AUTHOR("wynter");