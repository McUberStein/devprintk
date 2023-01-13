#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "printk"
#define MAX_DEVPRINTK_BUFFER 1024

static int devprintk_uevent(struct device*, struct kobj_uevent_env*);
static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

static int init_stat;
static int public = 0;
static char message_buffer[MAX_DEVPRINTK_BUFFER];
static struct class *cl;
static struct cdev c_dev;
static dev_t dev;

static struct file_operations ops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

static int devprintk_uevent(struct device *dev, struct kobj_uevent_env *env){
    add_uevent_var(env, "DEVMODE=%#o", 0777);
    return 0;
}

static int dev_open(struct inode *inodep, struct file *filep){
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
    return 0;
}

static ssize_t dev_read(struct file *filep, char *charp, size_t size, loff_t *off){
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *charp, size_t size, loff_t *off){
    memset(message_buffer, 0x0, sizeof(message_buffer));

    if(size > MAX_DEVPRINTK_BUFFER){
        size = MAX_DEVPRINTK_BUFFER;
    }
    if(copy_from_user(message_buffer, charp, size) != 0){
        printk(KERN_ALERT "Failed reading from /dev/printk\n");
        return 0;
    }
    printk(KERN_INFO "devprintk: %s", message_buffer);
    return size;
}

static int __init devprintk_init(void){
        init_stat = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
        if(init_stat < 0){
            printk(KERN_ALERT "Failed registering device\n");
            return init_stat;
        }
        if((cl = class_create(THIS_MODULE, "log")) == NULL){
            printk(KERN_ALERT "Class creation failed\n");
            unregister_chrdev_region(dev, 1);
            return -1;
        }
        if(public > 0){
            public = 1;
            cl->dev_uevent = devprintk_uevent;
        }
        if(device_create(cl, NULL, dev, NULL, DEVICE_NAME) == NULL){
            printk(KERN_ALERT "Device creation failed\n");
            class_destroy(cl);
            unregister_chrdev_region(dev, 1);
            return -1;
        }
        cdev_init(&c_dev, &ops);
        if(cdev_add(&c_dev, dev, 1) == -1){
            printk(KERN_ALERT "Device addition failed\n");
            device_destroy(cl, dev);
            class_destroy(cl);
            unregister_chrdev_region(dev, 1);
            return -1;
        }

        printk(KERN_INFO "devprintk: printk device module successfully loaded");
        printk(KERN_INFO "devprintk: loaded with public=%d", public);
        return 0;
}

static void __exit devprintk_exit(void){
        cdev_del(&c_dev);
        device_destroy(cl, dev);
        class_destroy(cl);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "printk device module unloaded\n");
}


module_init(devprintk_init);
module_exit(devprintk_exit);
module_param(public, int, 0660);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Me");
MODULE_DESCRIPTION("Print with printk");
