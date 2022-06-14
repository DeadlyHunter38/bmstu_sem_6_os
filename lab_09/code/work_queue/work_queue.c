#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/delay.h>

#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_SCANCODE_MASK 0x7F
#define KEYBOARD_STATUS_MASK 0x80

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");

static char *keyboard_codes[] = {
	"[ESC]", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "backSpace", "[Tab]", "Q",
    "W", "E", "R", "T", "Y", "U", "I", "O", "P", "[", "]", "[Enter]", "[CTRL]", "A", "S", "D",
    "F", "G", "H", "J", "K", "L", ";", "\'", "`", "[LShift]", "\\", "Z", "X", "C", "V", "B", "N", "M",
    ",", ".", "/", "[RShift]", "[PrtSc]", "[Alt]", " ", "[Caps]", "F1", "F2", "F3", "F4", "F5",
    "F6", "F7", "F8", "F9", "F10", "[Num]", "[Scroll]", "[Home(7)]", "[Up(8)]", "[PgUp(9)]", "-",
    "[Left(4)]", "[Center(5)]", "[Right(6)]", "+", "[End(1)]", "[Down(2)]", "[PgDn(3)]", "[Ins]", "[Del]"};

int dev_id;
struct workqueue_struct *work_queue;
struct work_struct keyboard_worker, sleep_worker;
char scancode, status;

char *keyboard_str;

void keyboard_worker_handler(struct work_struct *work)
{
    scancode = inb(KEYBOARD_DATA_PORT);

    keyboard_str = keyboard_codes[(scancode & KEYBOARD_SCANCODE_MASK) - 1];
    printk(KERN_INFO "workqueue: код %d %s.\n", scancode & 0x7F, keyboard_str);
}

void sleep_worker_handler(struct work_struct *work)
{
    printk(KERN_INFO "workqueue: sleep worker (переход в режим сна)");
	msleep(500);
	printk(KERN_INFO "workqueue: sleep worker (пробуждение после сна):");
}

irqreturn_t irq_handler(int irq, void *device)
{
    if (irq == KEYBOARD_IRQ)
    {
        queue_work(work_queue, &keyboard_worker);
        queue_work(work_queue, &sleep_worker);
        return IRQ_HANDLED;
    }

    return IRQ_NONE;
}

static int __init init_workqueue(void)
{
    int error = request_irq(KEYBOARD_IRQ, (irq_handler_t)irq_handler, IRQF_SHARED, "work_queue", &dev_id);
    if (error){
        printk(KERN_INFO "workqueue: обработчик прерывания от клавиатуры не был зарегистрирован.");
        return error;
    }

    work_queue = create_workqueue("work_queue");
    if (!work_queue)
    {
        free_irq(KEYBOARD_IRQ, &dev_id);
        printk(KERN_INFO "workqueue: очередь работ не была создана.");
        return -ENOMEM;
    }

    INIT_WORK(&keyboard_worker, keyboard_worker_handler);
    INIT_WORK(&sleep_worker, sleep_worker_handler);

    printk(KERN_INFO "workqueue: модуль ядра был загружен успешно.");
    return error;
}

static void __exit exit_workqueue(void)
{
    flush_workqueue(work_queue);
    destroy_workqueue(work_queue);
    free_irq(KEYBOARD_IRQ, &dev_id);
    printk(KERN_INFO "workqueue: модуль ядра был выгружен успешно.");
}

module_init(init_workqueue);
module_exit(exit_workqueue);