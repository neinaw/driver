/*Standard headers*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include <linux/seq_file.h> //for seq_printf()
			    //printk prints to kernel log
			    //seq_* APIs are used mostly when printing to /proc
			    //files as helper functions, thereby streamlining
			    //debugging through formatted output from proc_read callback

#include <linux/proc_fs.h> //for creating files under /proc and related structure definitions

#include <linux/jiffies.h> //jiffy is a kernel unit of time
			   //HZ is a constant which is the number of times jiffies is incremented in a second
			   //Each increment is called a tick
			   //HZ therefore is the size of a jiffy, and depends on the hardware and kernel version

#define STR_PRINTF_RET(len, str, args...) seq_printf(m, str, ## args)

static struct proc_dir_entry *parent, *file, *link;//in linux/proc_fs.h
static int state = 0;

static int time_read(struct seq_file *m, void *v){
    //function provides the current state and the time since 
    //the system has been booted u in different units based on the 
    //current state.
    //state	unit
    //  0	jiffies
    //  1	milliseconds
    //  2	seconds and milliseconds
    //  3	hours:minutes:seconds
    int len = 0, val;
    unsigned long act_jiffies;

    STR_PRINTF_RET(len, "state = %d\n", state);
    act_jiffies = jiffies - INITIAL_JIFFIES;
    val = jiffies_to_msecs(act_jiffies);
    switch (state){
        case 0:
            STR_PRINTF_RET(len, "uptime = %ld jiffies\n", act_jiffies);
            break;
        case 1:
            STR_PRINTF_RET(len, "uptime = %d msecs\n", val);
            break;
        case 2:
            STR_PRINTF_RET(len, "uptime = %ds %dms\n",
                    val / 1000, val % 1000);
            break;
        case 3:
            val /= 1000;
            STR_PRINTF_RET(len, "uptime = %02d:%02d:%02d\n",
                    val / 3600, (val / 60) % 60, val % 60);
            break;
        default:
            STR_PRINTF_RET(len, "<undefined state %d>\n", state);
            break;
    }

    return len;
}
static ssize_t time_write(struct file *file, const char __user *buffer, size_t count, loff_t *off){
    char kbuf[2];

    //similar to character device file operation write()
    //allows to set the "state" of the file from userspace

    if (count > 2)
        return count;
    if (copy_from_user(kbuf, buffer, count)){
        return -EFAULT;
    }
    if ((count == 2) && (kbuf[1] != '\n'))
        return count;
    if ((kbuf[0] < '0') || ('9' < kbuf[0]))
        return count;
    state = kbuf[0] - '0';
    return count;
}

static int time_open(struct inode *inode, struct file *file){
    return single_open(file, time_read, NULL);
}

static struct proc_ops fops = { //similar to struct file_operations to specify what to do
				//when open(), read(), etc are called on the file
    .proc_open = time_open,
    .proc_read = seq_read,
    .proc_write = time_write
};

static int __init proc_win_init(void){
    if ((parent = proc_mkdir("my_dir", NULL)) == NULL){
	/*
	struct proc_dir_entry *proc_mkdir(const char *dir_name, proc_dir_entry *parent);
	returns pointer to the created parent.
	NULL as the second argument makes parent = my_dir
	*/
        return -1;
    }
    if ((file = proc_create("rel_time", 0666, parent, &fops)) == NULL){
	/*
	struct proc_dir_entry *proc_create(const char *name, 
					   struct proc_dir_entry *parent,
					   const struct proc_ops *proc_ops);
	
	Similar to class_create for character drivers
	*/
        remove_proc_entry("my_dir", NULL); //reverse call of proc_mkdir()
        return -1;
    }
    if ((link = proc_symlink("rel_time_l", parent, "rel_time")) == NULL){
	/*
	struct proc_dir_entry *proc_symlink(const char *from,
					    struct proc_dir_entry *parent,
					    const char *to);
	*/

	//Inverse calls
        remove_proc_entry("rel_time", parent);
        remove_proc_entry("anil", NULL);
        return -1;
    }
    proc_set_user(link, KUIDT_INIT(0), KGIDT_INIT(100));
    return 0;
}

static void __exit proc_win_exit(void){
    remove_proc_entry("rel_time_l", parent);
    remove_proc_entry("rel_time", parent);
    remove_proc_entry("my_dir", NULL);
}

module_init(proc_win_init);
module_exit(proc_win_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ansh Waikar");
MODULE_DESCRIPTION("Kernel Window /proc Demonstration Driver");
