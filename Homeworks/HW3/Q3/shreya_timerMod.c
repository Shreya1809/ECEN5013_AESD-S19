#include <linux/module.h>  // Core header for loading LKMs into the kernel
#include <linux/moduleparam.h> //To allow arguments to be passed to your module
#include <linux/kernel.h>  // Contains types, macros, functions for the kernel
#include <linux/init.h> // Macros used to mark up functions e.g., __init __exit
#include <linux/timer.h> //time management in kernel,jiffies
#include <linux/delay.h> //for delay
#include <linux/stat.h> //permissions for module param

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Shreya Chakraborty");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple timer based Linux driver for the BBG.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static char *name = "shreya";        ///< An example LKM argument, the param variable has to be declared static.
static unsigned long timeout = 500; //default timeout value
static unsigned long expire_time; // for calculation of expiration time
static unsigned long count = 0; // to keep track of no of times timer function called
struct timer_list k_timer; //Timers are defined by the timer_list structure

module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description
module_param(timeout, ulong, S_IRUGO); ///< Param desc. ulong = unsigned long, S_IRUGO can be read/not changed
MODULE_PARM_DESC(timeout, "The period in miliseconds");  ///< parameter description

void _TimerHandler(struct timer_list  *timer) // call back function for  resetting the timer
{
    count++;
    expire_time = jiffies + msecs_to_jiffies(timeout);
    /*Restarting the timer...*/
    mod_timer( &k_timer, expire_time);
 
    printk(KERN_INFO "Timer Handler called\n Name: %s\n No of times : %lu\n",name,count); //print name and the no of times callback function called in kernel log
}

static int __init mytimer_init(void)
{
    printk(KERN_INFO "shreya_timerMod inserted into kernel.Function %s\n",__FUNCTION__);
    printk(KERN_INFO "The timeout is : %lu milliseconds\n",timeout);
    expire_time = jiffies + msecs_to_jiffies(timeout);
    /*Starting the timer.*/
    //setup_timer(&k_timer, _TimerHandler, 0);//the call back function will be called every timeout.
    timer_setup(&k_timer, _TimerHandler, 0);
    add_timer(&k_timer); //adding the timer to the list of timers
    mod_timer( &k_timer, expire_time); // modifying the timer to start
 
    return 0;
}

static void __exit mytimer_exit(void)
{
    count = 0;
    del_timer(&k_timer);
    printk(KERN_INFO "shreya_timerMod exited from kernel. Function %s\n",__FUNCTION__);
}


module_init(mytimer_init);
module_exit(mytimer_exit);
