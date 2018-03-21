#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/notifier.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
MODULE_LICENSE("GPL");

extern int test_notifier_call_chain(unsigned long val, void *v);

static int __init call_notifier(void){
	int err;
	printk("Begin to notify:\n");
	
	printk("===================================\n");
	err = test_notifier_call_chain(1, NULL);
	printk("===================================\n");
	
	if (err){
		printk("notifier_call_chain error\n");
	}
	
	return err;
}

static void __exit uncall_notifier(void){
	printk("End notify\n");
}

module_init(call_notifier);
module_exit(uncall_notifier);

