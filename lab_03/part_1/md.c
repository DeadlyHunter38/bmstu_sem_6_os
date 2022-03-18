#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");

static int __init md_init(void)
{
    struct task_struct *task = &init_task;
    printk(KERN_INFO "+ Module is loaded.");
    do
    {
        printk(KERN_INFO "pid = %d, name = %s, ppid = %d, pname = %s, state = %lu, prio = %d\n",
               task->pid, task->comm, task->parent->pid, task->parent->comm,
               task->state, task->prio);
    } while ((task = next_task(task)) != &init_task);

    printk(KERN_INFO "Current: pid = %d, name = %s, ppid = %d, pname = %s, state = %lu, prio = %d\n", 
           current->pid, current->comm, current->parent->pid, current->parent->comm,
           task->state, task->prio);    
    return 0;    
}

static void __exit md_exit(void){
    printk(KERN_INFO "+ Module is unloaded.");
    printk(KERN_INFO "Current: pid = %d, name = %s, ppid = %d, pname = %s, state = %lu, prio = %d\n", 
           current->pid, current->comm, current->parent->pid, current->parent->comm,
           task->state, task->prio);  
}

module_init(md_init);
module_exit(md_exit);