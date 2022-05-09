#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");

#define MAX_COOKIE_SIZE PAGE_SIZE
#define PROC_FILE_NAME "sequence"
#define PROC_DIR_NAME "dir_sequence"
#define PROC_SYMLINK_NAME "symlink_sequence"
#define FILE_PATH PROC_FILE_NAME 

static struct proc_dir_entry *proc_file = NULL;
static struct proc_dir_entry *proc_dir = NULL;
static struct proc_dir_entry *proc_link = NULL;

static char temp_buffer[MAX_COOKIE_SIZE];

static char *cookie_pot;
static int write_index;
static int read_index;

static int seq_file_open(struct inode *inode, struct file *file);
static ssize_t seq_file_write(struct file *filep, const char __user *buf, size_t len, loff_t *offp);

static void *seq_file_start(struct seq_file *s, loff_t *pos);
static void *seq_file_next(struct seq_file *s, void *v, loff_t *pos);
static void seq_file_stop(struct seq_file *s, void *v);
static int seq_file_show(struct seq_file *s, void *v);

static struct seq_operations seq_file_ops = {
    .start = seq_file_start,
    .next = seq_file_next,
    .stop = seq_file_stop,
    .show = seq_file_show
};

static const struct proc_ops proc_file_ops = {
    .proc_open = seq_file_open,
    .proc_release = seq_release,
    .proc_write = seq_file_write,
    .proc_read = seq_read
};

static int seq_file_open(struct inode *inode, struct file *file){
    printk(KERN_INFO "seq: open.\n");
    return seq_open(file, &seq_file_ops);
}

static ssize_t seq_file_write(struct file *file, const char __user *buffer,
                             size_t len, loff_t *offset)
{
    int available_space = (MAX_COOKIE_SIZE - write_index) + 1;

    if (len > available_space)
    {
        printk(KERN_INFO "seq: cookie_pot is full.\n");
        return -ENOSPC;
    }

    if (copy_from_user(&cookie_pot[write_index], buffer, len)){
        printk(KERN_ERR "seq: copy_from_user error.\n");
        return -EFAULT;
    }

    printk(KERN_INFO "seq: write.\n");
    write_index += len;
    cookie_pot[write_index - 1] = '\0';

    return len;
}

static void *seq_file_start(struct seq_file *s, loff_t *pos){
    printk(KERN_INFO "seq: start.\n");

    static unsigned long counter = 0;
    if (!*pos){
        return &counter;
    }

    *pos = 0;
    return NULL;
}

static void *seq_file_next(struct seq_file *s, void *v, loff_t *pos)
{
    printk(KERN_INFO "seq: next.\n");
    unsigned long *temp_v = (unsigned long *)v;
    (*temp_v)++;
    (*pos)++;

    return NULL;
}

static void seq_file_stop(struct seq_file *s, void *v){
    printk(KERN_INFO "seq: stop.\n");
}

static int seq_file_show(struct seq_file *s, void *v)
{
    int len;
    printk(KERN_INFO "seq: show.\n");

    if (!write_index){
        return 0;
    }

    if (read_index >= write_index){
        read_index = 0;
    }

    len = snprintf(temp_buffer, MAX_COOKIE_SIZE, "%s\n", &cookie_pot[read_index]);

    seq_printf(s, "%s", temp_buffer);
    read_index += len;
    return 0;
}

static int __init seq_proc_module_init(void)
{
    cookie_pot = (char *)vmalloc(MAX_COOKIE_SIZE);

    if (!cookie_pot){
        return -ENOMEM;
    }

    memset(cookie_pot, 0, MAX_COOKIE_SIZE);

    proc_file = proc_create(PROC_FILE_NAME, 0644, NULL, &proc_file_ops);
    if (!proc_file){
        printk(KERN_INFO "seq: couldn't create proc file.\n");
        return -ENOMEM;
    }

    proc_dir = proc_mkdir(PROC_DIR_NAME, NULL);
    if (!proc_dir){
        printk(KERN_INFO "seq: couldn't create proc dir.\n");
        return -ENOMEM;
    }

    proc_link = proc_symlink(PROC_SYMLINK_NAME, NULL, FILE_PATH);
    if (!proc_link){
        printk(KERN_INFO "seq: couldn't create proc symlink.\n");
        return -ENOMEM;
    }

    write_index = 0;
    read_index = 0;
    printk(KERN_INFO "seq: module is loaded.\n");
    return 0;
}

static void __exit seq_proc_module_exit(void){
    if (proc_file){
        remove_proc_entry(PROC_FILE_NAME, NULL);
    }
    if (proc_dir){
        remove_proc_entry(PROC_DIR_NAME, NULL);
    }
    if (proc_link){
        remove_proc_entry(PROC_SYMLINK_NAME, NULL);
    }
    vfree(cookie_pot);
    printk(KERN_INFO "seq: module is unloaded.\n");
}

module_init(seq_proc_module_init);
module_exit(seq_proc_module_exit);