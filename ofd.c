#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/types.h>

static dev_t first; //defined in <linux/types.h>, contains both major and minor numbers.

int ofd_init(void) /* Constructor */{
    printk(KERN_INFO "Namaskar: ofd registered\n"); //prints to /var/log/syslog
    int ret;
    if((ret = alloc_chrdev_region(&first,0,3,"test"))<0){ //unsuccessful allocation
	return ret;
    }
    printk(KERN_INFO "<Major, Minor>: <%d,%d>\n",MAJOR(first),MINOR(first));
    return 0;
}

void ofd_exit(void) /* Destructor */{
    printk(KERN_INFO "Alvida: ofd unregistered\n");
    unregister_chrdev_region(first,3);
}

module_init(ofd_init);
module_exit(ofd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ansh Waikar");
MODULE_DESCRIPTION("Our First Driver");
