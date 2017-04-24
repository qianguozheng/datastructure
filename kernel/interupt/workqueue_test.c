#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/delay.h>


static struct workqueue_struct *queue = NULL;
static struct work_struct work;
static struct delayed_work test_dwq;

void delay_func(struct work_struct *work){
    int i;

    printk(KERN_INFO"My name is delay_func!\n");
    for (i = 0; i < 3; i++){
        printk(KERN_ERR"delay_fun: i=%d\n", i);
        msleep(100);
    }
}

static void work_handler(struct work_struct *data){
    printk(KERN_ALERT"work handler functions. current->pid=%d\n", current->pid);
}

static int __init test_init(void){
    int i, ret;

    queue = create_workqueue("test_wq");
    if (!queue)
        goto err;
    
    printk(KERN_INFO"Create workqueue successful\n");
    INIT_DELAYED_WORK(&test_dwq, delay_func);

    ret = queue_delayed_work(queue, &test_dwq, 5000);
    printk(KERN_INFO"first ret=%d\n", ret);

    for (i=0; i<3; i++){
        printk(KERN_INFO"Example:ret=%d,i=%d\n", ret, i);
        msleep(100);
    }

    ret = queue_delayed_work(queue, &test_dwq, 0);
    printk(KERN_INFO"second ret=%d\n", ret);

    return 0;
err:
    return -1;
}

static void __exit test_exit(void){
    int ret;
    ret = cancel_delayed_work(&test_dwq);
    flush_workqueue(queue);
    destroy_workqueue(queue);
}

MODULE_LICENSE("GPL");
module_init(test_init);
module_exit(test_exit);
