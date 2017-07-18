#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> //Required for various structures related to files liked fops.
#include <linux/semaphore.h>
#include <linux/cdev.h>
#include <linux/version.h>

#include <linux/device.h> //class_create

#include <linux/ioctl.h>
#define IOC_MAGIC 'k'
#define IOCTL_HELLO _IO(IOC_MAGIC,0)

static int Major;
int open(struct inode *inode, struct file *filp){
	printk(KERN_INFO "Inside open\n");
	return 0;
}

int release (struct inode *inode, struct file *filp){
	printk(KERN_INFO "Inside close\n");
	return 0;
}

int ioctl_funcs(struct file *filp, unsigned int cmd, unsigned long arg){
	int data = 10, ret;
	
	switch(cmd){
		case IOCTL_HELLO:
			printk(KERN_INFO"Hello ioctl world");
			break;
	}
	
	return ret;
}

struct file_operations fops = {
	open: open,
	unlocked_ioctl: ioctl_funcs, //2.6.XX replace ioctl with unlocked_ioctl to remote BLK
	release: release
};

#if 0
//COMMENT: Need add char dev by hand in /dev folder
//sudo mknod /dev/temp c 244 0
struct cdev *kernel_cdev;

int char_arr_init(void){
	int ret;
	dev_t dev_no, dev;
	kernel_cdev = cdev_alloc();
	kernel_cdev->ops = &fops;
	kernel_cdev->owner = THIS_MODULE;
	
	printk("Inside init module\n");
	ret = alloc_chrdev_region(&dev_no, 0, 1, "char_arr_dev");
	if (ret < 0){
		printk("Major number allocation is failed\n");
		return ret;
	}
	
	Major = MAJOR(dev_no);
	dev = MKDEV(Major, 0);
	printk(" The major number for your device is %d\n", Major);
	ret = cdev_add(kernel_cdev, dev, 1);
	if (ret < 0){
		printk(KERN_INFO"Unable to allocation dev");
		return ret;
	}
}

void char_arr_cleanup(void){
	printk(KERN_INFO"Inside cleanup_module\n");
	cdev_del(kernel_cdev);
	unregister_chrdev_region(Major, 1);
}
#else
//Auto generate device
//Refer to http://blog.csdn.net/lwj103862095/article/details/17470573

static struct class *char_class;
static struct device *char_device;
static int char_arr_init(void){
	
	//主设备号为0,表示由系统哦个自动分配主设备号
	Major = register_chrdev(0, "char_arr_dev", &fops);
	//创建chardrv类
	char_class = class_create(THIS_MODULE, "chardrv");
	if (NULL == char_class){
		printk("Create char class failed\n");
		return -1;
	}
	//在chardrv类下创建设备, 供应用程序打开设备
	char_device = device_create(char_class, NULL, MKDEV(Major, 0), NULL, "temp");
	if (NULL == char_device){
		printk("Create device failed\n");
		return -1;
	}
	return 0;
}
static void char_arr_cleanup(void){
	
	unregister_chrdev(Major, "char_arr_dev");
	if (char_class){
		device_unregister(char_device);
	}
	if (char_device){
		class_destroy(char_class);
	}
}
#endif
MODULE_LICENSE("GPL");
module_init(char_arr_init);
module_exit(char_arr_cleanup);

