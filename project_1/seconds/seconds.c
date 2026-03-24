#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>
#include <linux/param.h>

#define PROC_NAME "seconds"
#define BUFFER_SIZE 128

static unsigned long start_time = 0;

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static const struct proc_ops seconds_ops = {
    .proc_read = proc_read,
};

static int __init seconds_init(void) {
    start_time = jiffies;
    proc_create(PROC_NAME, 0666, NULL, &seconds_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit seconds_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

static ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    rv = sprintf(buffer, "%lu seconds has past since the module loaded.\n", (jiffies - start_time) / HZ);

    if (copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT;
    }

    return rv;
}
module_init(seconds_init);
module_exit(seconds_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("wynter");