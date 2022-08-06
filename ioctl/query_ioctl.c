#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>

#include "query_ioctl.h"

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static int var_1 = 1, var_2 = 2, var_3 = 3; //device file variables to be queried

static int my_open(struct inode *i, struct file *f){
    return 0;
}
static int my_close(struct inode *i, struct file *f){
    return 0;
}
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
    /*******IMP*********
    In kernel, unsigned long is often used as a substitution for pointers,
    as pointers always have that size on every architecture.
    So generic memory addresses in the kernel are usually unsigned long.
    Treat arg as a pointer
    */
    query_arg_t q;

    switch (cmd){
	case QUERY_GET_VARIABLES:
	    q.var_1 = var_1;
	    q.var_2 = var_2;
	    q.var_3 = var_3;
	    if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t))){//typecast arg to query_arg_t
		return -EACCES;
	    }
	    break;
	case QUERY_CLR_VARIABLES:
	    var_1 = 0;
	    var_2 = 0;
	    var_3 = 0;
	    break;
	case QUERY_SET_VARIABLES:
	    if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t))){
		return -EACCES;
	    }
	    var_1 = q.var_1;
	    var_2 = q.var_2;
	    var_3 = q.var_3;
	    break;
	default:
	    return -EINVAL;
    }
    return 0;
}

static struct file_operations query_fops ={
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl
};

/*
class name: char
char_dev name: query_ioctl
dev name: query
*/

static int __init query_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;

    if ((ret = alloc_chrdev_region(&dev, 0, 1, "query_ioctl")) < 0){
	return ret;
    }

    cdev_init(&c_dev, &query_fops);

    if ((ret = cdev_add(&c_dev, dev, 1)) < 0){
	return ret;
    }
    
    if (IS_ERR(cl = class_create(THIS_MODULE, "char"))){
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, 1);
	return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query"))){
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, 1);
	return PTR_ERR(dev_ret);
    }
    return 0;
}

static void __exit query_ioctl_exit(void){
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, 1);
}

module_init(query_ioctl_init);
module_exit(query_ioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ansh Waikar");
MODULE_DESCRIPTION("Query ioctl() Char Driver");
