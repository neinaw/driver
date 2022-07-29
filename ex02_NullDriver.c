#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h> // protects against kernel oops.

static dev_t first; //defined in <linux/types.h>, contains both major and minor numbers.
static struct cdev c_dev; // global character device structure variable
static struct class *cl; // global device class variable

static char c; // preserving the last character

static int my_open(struct inode *i, struct file *f){
    printk(KERN_INFO "Driver: open()\n");
    return 0;
}

static int my_close(struct inode *i, struct file *f){
    printk(KERN_INFO "Driver: close()\n");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off){
    /*wrties upto <len> bytes into buf when read() is called*/
    printk(KERN_INFO "Driver: read()\n");
    if (*off == 0){
        if (copy_to_user(buf, &c, 1) != 0)
            return -EFAULT;
        else{
            (*off)++;
            return 1;
        }
    }
    else
        return 0;
}

static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off){ // reverse of read; writes to buf
    printk(KERN_INFO "Driver: write()\n");
    if (copy_from_user(&c, buf+len-1,1) != 0)
	return -EFAULT;
    else
	return len;
}

static struct file_operations pugs_fops = { // specify what the read(), write(), etc. functions actually are. (Defined above)
    .owner = THIS_MODULE,                   // exectued after the functions are called.
    .open = my_open,
    .release = my_close,
    .read = my_read,
    .write = my_write
};

int ofd_init(void) /* Constructor */{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "Namaskar: ofd registered\n"); //prints to /var/log/syslog
    if ((ret = alloc_chrdev_region(&first,0,3,"test"))<0){ //unsuccessful allocation
	return ret;
    }
    if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv"))){
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "mynull"))){
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &pugs_fops);

    if ((ret = cdev_add(&c_dev, first, 1)) < 0){
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }
    return 0;
}

void ofd_exit(void) /* Destructor */{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Alvida: ofd unregistered\n");
}

module_init(ofd_init);
module_exit(ofd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ansh Waikar");
MODULE_DESCRIPTION("Our First Driver");
