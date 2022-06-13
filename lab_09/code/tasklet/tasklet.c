#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#define KEYBOARD_IRQ 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");

struct tasklet_struct tasklet;

void tasklet_worker(unsigned long data){
    printk(KERN_INFO "tasklet: state - %ld; count - %ld, data - %ld\n",
           tasklet.state, tasklet.count, tasklet.data);
}

irqreturn_t irq_handler(int irq_number, void *device)
{
    if (irq_number == KEYBOARD_IRQ){
        tasklet_schedule(&tasklet);
        printk(KERN_INFO "tasklet: прерывание; state - %ld; count - %ld, data - %ld\n",
               tasklet.state, tasklet.count, tasklet.data);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static int __init init_tasklet(void){
    int error = request_irq(KEYBOARD_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED, 
                            "keyboard_handler", (void*)(irq_handler));
    if (error){
        printk(KERN_INFO "tasklet: обработчик прерывания от клавиатуры не был зарегистрирован.");
        return error;
    }

    printk(KERN_INFO "tasklet: обработчик прерывания от клавиатуры был зарегистрирован успешно.");
    printk(KERN_INFO "tasklet: модуль ядра был загружен успешно.");

    tasklet_init(&tasklet, tasklet_worker, 12);
    return error;
}

static void __exit exit_tasklet(void){
    free_irq(KEYBOARD_IRQ, (void *)(irq_handler));
	tasklet_disable(&tasklet);
	tasklet_kill(&tasklet);
	printk(KERN_DEBUG "tasklet: модуль ядра выгружен\n");
}

module_init(init_tasklet);
module_exit(exit_tasklet);