/* Header file to be included by modules using the exported symbols my_class and get_my_class */

#ifndef EX05_EXPORTSYM_H
#define EX05_EXPORTSYM_H

#ifdef __KERNEL__
#include <linux/device.h>

//extern is used similar to when global variables are defined in header files
extern struct class *my_class; 
extern struct class *get_my_class(void);
#endif

#endif

/*
When the kernel is compiled, the __KERNEL__ macro
is defined on the command line
User-space programs need access to kernel headers, but 
some info in them is intended only for the kernel, and
therefore wrapping them in the ifdef--endif block ensures
that userspace programs don't see that block
*/
