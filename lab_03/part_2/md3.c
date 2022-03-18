#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");

//extern char *md1_noexport(void);
//extern char *md1_local(void);

static int __init md_init(void)
{
    printk("+ module md2 start!\n");
    printk("+ data string exported from md1 : %s\n", md1_data);
    printk("+ string returned md1_proc() is : %s\n", md1_proc());

    //printk("+ local from md1: %s\n", md1_local());
    //printk("+ noexport from md1: %s\n", md1_noexport());
    return -1;
}

module_init(md_init);