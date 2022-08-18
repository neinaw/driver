#include <linux/module.h>
#include <linux/device.h>

/* Note that declaring the follow to be "static" contradicts EXPORT_SYMBOL */
struct class *my_class;
struct class *get_my_class(void){
    return my_class;
}

EXPORT_SYMBOL(my_class);
EXPORT_SYMBOL_GPL(get_my_class);

static int __init glob_sym_init(void){
    if (IS_ERR(my_class = class_create(THIS_MODULE, "null"))){
        return PTR_ERR(my_class);
    }
    return 0;
}

static void __exit glob_sym_exit(void){
    class_destroy(my_class);
}

module_init(glob_sym_init);
module_exit(glob_sym_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ansh Waikar");
MODULE_DESCRIPTION("Global Symbols Exporting Driver");
