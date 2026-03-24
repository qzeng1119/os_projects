#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h> // instead of using <asm/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "hello"

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

// using *proc_ops* instead of *file_operations*
static const struct proc_ops my_p_ops = {
    .proc_read = proc_read,
    //.owner     = THIS_MODULE,  // just abandon this line
};

static int __init hello_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &my_p_ops);
    printk(KERN_INFO "/proc/%s created\n", PROC_NAME);
    return 0;
}

static void __exit hello_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    
    if (completed) {
        completed = 0; // This variable exits with the module, to avoid next "cat" being ineffective, we should set it back to 0 before return.
        return 0;
    }

    completed = 1;

    rv = sprintf(buffer, "Hello World\n");

    // copy_to_user(usr_buf, buffer, rv);
    // because copy_to_user is a high-risk function, so the returned value must be checked.
    if (copy_to_user(usr_buf, buffer, rv)) {
        return -EFAULT; // address error
    }

    return rv;
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("wynter");