#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/init_task.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/time.h>

#define MAGIC_NUMBER 0x13131313
#define SLAB_NAME "fs_slab"
#define CACHE_OBJECTS_AMOUNT 32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tartykov Lev");
MODULE_DESCRIPTION("Example of vfs.");

static struct kmem_cache *cache = NULL; 
static void* *line = NULL; 

struct fs_inode {
    int           i_mode;
    unsigned long i_ino;
};

static struct fs_inode **inode_ptrs = NULL;

static void fs_put_super(struct super_block *sb){
    printk(KERN_INFO "example_vfs: super block is destroyed.");
}

static struct super_operations fs_super_ops = {
    .put_super = fs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode
};

static int fs_fill_sb(struct super_block *sb, void *data, int silent)
{
    struct dentry *root;
    struct inode *inode;

    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MAGIC_NUMBER;
    sb->s_op = &fs_super_ops;

    inode = new_inode(sb);
    printk(KERN_INFO "example_vfs: inode = %lu", inode->i_ino);
	if (!inode)
		return -ENOMEM;

    inode->i_ino = 15;
	inode->i_mode = S_IFDIR | 0755;
	inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
	inode->i_op = &simple_dir_inode_operations;
	inode->i_fop = &simple_dir_operations;

    root = d_make_root(inode);
    if (!root){
        printk(KERN_ERR "example_vfs: inode allocation failed.");
        return -ENOMEM;
    }

    sb->s_root = root;

    return 0;
}

static struct dentry *fs_mount(struct file_system_type *type, int flags, char const *dev, void *data)
{
    struct dentry *const entry = mount_nodev(type, flags, data, fs_fill_sb);
    if (IS_ERR(entry)){
        printk(KERN_ERR "example_vfs: mounting is denied.");
    }
    else{
        printk(KERN_INFO "example_vfs: mounting is successful.");
    }

    return entry;
}

static void kill_super_block(struct super_block *sb){
    printk(KERN_INFO "example_vfs: kill super block.\n");
    kill_anon_super(sb);
}

static struct file_system_type fs_type = {
    .name = "myfs",
    .owner = THIS_MODULE,
    .mount = fs_mount,
    .kill_sb = kill_super_block,
    .fs_flags = FS_USERNS_MOUNT
};

static int __init fs_init(void){
    int result_register = register_filesystem(&fs_type);

    if (result_register != 0){
        printk(KERN_ERR "example_vfs: can't register file system.");
        return result_register;
    }

    if ((line = kmalloc(sizeof(void*) * CACHE_OBJECTS_AMOUNT, GFP_KERNEL)) == NULL)
    {
        printk(KERN_ERR "example_vfs: failed to allocate memory.\n");
        return -ENOMEM;
    }

    if ((cache = kmem_cache_create(SLAB_NAME, 7, 0, SLAB_HWCACHE_ALIGN, NULL)) == NULL)
    {
        printk(KERN_ERR "example_vfs: failed to create cache.\n");
        kmem_cache_destroy(cache); 
        return -ENOMEM;
    }

    printk(KERN_INFO "example_vfs: module is loaded.");
    return 0;
}

static void __exit fs_exit(void){
    kmem_cache_destroy(cache);

    int result_unregister = unregister_filesystem(&fs_type);

    if (result_unregister != 0){
        printk(KERN_ERR "example_vfs: can't unregister file system.");
    }
    else{
        printk(KERN_INFO "example_vfs: module is unloaded.");
    }
}

module_init(fs_init);
module_exit(fs_exit);