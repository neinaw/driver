//declare all variables as static because even the 
//smallest module is linked against the entire kernel,
//to reduce namespace pollution

#include <linux/module.h> //For module_init(), module_exit(), MODULE_* macros
#include <linux/version.h> //Included for version compatibility of the module with the kernel  
#include <linux/kernel.h> //Needed for printk alert levels like KERN_INFO, KERN_ALERT, etc.
#include <linux/init.h> // for __init and __exit macros
#include <linux/fs.h> //for alloc_chrdev_register(), unregister_chrdev_region(), etc. APIs
#include <linux/kdev_t.h> //for MAJOR(dev_t ), MINOR(dev_t ) macros to extract major/minor numbers
#include <linux/types.h> // struct dev_t definition

static dev_t first; //contains both major and minor numbers.

static int __init ofd_init(void) /* Constructor */{
    /*
    __init has no effect on modules
    causes the init function to be discarded and its memory 
    freed once the init function finishes for built-in drivers
    */
    int ret;
    printk(KERN_INFO "Namaskar: ofd registered\n"); //prints to /var/log/syslog
    if((ret = alloc_chrdev_region(&first,0,3,"test"))<0){
	/*
	int alloc_chrdev_region(dev_t *dev, 
				unsigned baseminor, 
				unsigned count, 
				const char *name) 
	dynamically figures out the first free major #
	dev: pointer to dev_t struct
	baseminor: first of the minor number to be allocated
	count: number of minors to be allocated
	name: name of the character driver
	returns 0 for successful allocation
	returns negative for unsuccessful allocation
	*/
	return ret;
    }
    printk(KERN_INFO "<Major, Minor>: <%d,%d>\n",MAJOR(first),MINOR(first));
    return 0;
}

static void __exit ofd_exit(void) /* Destructor */{
    /*
    __exit has no effect on modules
    causes the ommission of the function when the module is built
    into the kernel
    */
    printk(KERN_INFO "Alvida: ofd unregistered\n");
    unregister_chrdev_region(first,3);
    /*
    unregister_chrdev_region(dev_t from,
			     unsinged count)
    from: first in the range of numbers to unregister
    count: number of devices to unregister
    */
}

module_init(ofd_init);
module_exit(ofd_exit);

//use modinfo <kernel_object_name>.ko to see info
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ansh Waikar");
MODULE_DESCRIPTION("Our First Driver");
