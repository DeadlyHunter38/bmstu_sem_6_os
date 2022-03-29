#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");

#define MAX_COOKIE_SIZE PAGE_SIZE

static struct proc_dir_entry *proc_entry;

static char *cookie_pot;
static int write_index;
static int read_index;

static char temp_buffer[MAX_COOKIE_SIZE];

static int fortune_open(struct inode *inode, struct file *file);
static int fortune_release(struct inode *inode, struct file *file);
static ssize_t fortune_write(struct file *, const char __user *,
                             size_t, loff_t *);
static ssize_t fortune_read(struct file *, char __user *,
                            size_t, loff_t *);

static const struct proc_ops proc_file_ops =
{
    .proc_open = fortune_open,
    .proc_release = fortune_release,
    .proc_write = fortune_write,
    .proc_read = fortune_read
};

static int fortune_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "fortune: open module.\n");
    return 0;
}

static int fortune_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "fortune: release module.\n");
    return 0;
}

static ssize_t fortune_write(struct file *file, const char __user *buffer,
                             size_t count, loff_t *offset)
{
    int available_space = (MAX_COOKIE_SIZE - write_index) + 1;

    if (count > available_space)
    {
        printk(KERN_INFO "fortune: cookie_pot is full.\n");
        return -ENOSPC;
    }

    if (copy_from_user(&cookie_pot[write_index], buffer, count)){
        printk(KERN_ERR "fortune: copy_from_user error.\n");
        return -EFAULT;
    }

    printk(KERN_INFO "fortune: count - %ld", count);
    write_index += count;
    cookie_pot[write_index - 1] = 0;

    return count;
}

static ssize_t fortune_read(struct file *file, char __user *buffer,
                             size_t count, loff_t *file_offset)
{
    int len;
    if (*file_offset > 0){
        return 0;
    }

    if (read_index >= write_index){
        read_index = 0;
    }

    len = snprintf(temp_buffer, MAX_COOKIE_SIZE, "%s\n", &cookie_pot[read_index]);

    if (copy_to_user(buffer, temp_buffer, len)){
        printk(KERN_ERR "fortune: copt_to_user error.\n");
        return -EFAULT;
    }

    read_index += len;
    *file_offset += len;
    return len;
}

static int __init fortune_module_init(void)
{
    int error = 0;
    cookie_pot = (char *)vmalloc(MAX_COOKIE_SIZE);

    if (!cookie_pot){
        error = -ENOMEM;
    }
    else
    {
        memset(cookie_pot, 0, MAX_COOKIE_SIZE);
        proc_entry = proc_create("fortune", 0644, NULL, &proc_file_ops);

        if (proc_entry == NULL)
        {
            error = -ENOMEM;
            vfree(cookie_pot);
            printk(KERN_INFO "fortune: couldn't create proc entry.\n");
        }
        else
        {
            write_index = 0;
            read_index = 0;

            proc_mkdir("dir_fortune", NULL);
            proc_symlink("symlink_fortune", NULL, "/proc/fortune");
            printk(KERN_INFO "fortune: module is loaded.\n");
        }
    }

    return error;
}

static void __exit fortune_module_exit(void)
{
    remove_proc_entry("fortune", NULL);
    remove_proc_entry("symlink_fortune", NULL);
    remove_proc_entry("dir_fortune", NULL);
    vfree(cookie_pot);
    printk(KERN_INFO "fortune module is unloaded.\n");
}





module_init(fortune_module_init);
module_exit(fortune_module_exit);