#include <linux/module.h>
#include <linux/init.h>

#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/io.h>
#include <linux/mm.h>
#include <linux/fs.h>

#include <linux/device.h>

#define MAJOR_NUM 990
#define MM_SIZE 4096

static char driver_name[] = "/dev/weeds_mmap";
static int dev_major = MAJOR_NUM;
static int dev_minor = 0;

char *buf = NULL;
struct cdev *cdev = NULL;

static int device_open(struct inode *inode, struct file *file){
	printk(KERN_ALERT"device open\n");
	buf = (char *)kmalloc(MM_SIZE, GFP_KERNEL); //内核申请内存只能按页申请，申请该内存以便后面把它当作虚拟设备
	return 0;
}

static int device_close(struct inode *inode, struct file *file)
{
	printk("device close\n");
	if (buf)
	{
		kfree(buf);
	}
	return 0;
}

static int device_mmap(struct file *file, struct vm_area_struct *vma)
{
	vma->vm_flags |= VM_IO;//表示对设备io空间的映射
	//从linux 3.7.0开始内核不再支持VM_RESERVED
	//vma->vm_flags |= VM_RESERVED;//标志盖内存区不能被换出，在设备驱动中虚拟页和物理页面的关系应该是长期的，应该保存起来，不能随便被别的虚拟页面换出
	vma->vm_flags |= VM_LOCKED|(VM_DONTEXPAND|VM_DONTDUMP);
	if (remap_pfn_range(vma, 
						vma->vm_start,//虚拟空间的起始地址 
						virt_to_phys(buf) >> PAGE_SHIFT, //与物理内存对应的页帧号，物理地址右移12位 
						vma->vm_end - vma->vm_start, //映射区域大小，一般是页大小的整数倍
						vma->vm_page_prot //保护属性
						))
	{
		return -EAGAIN;
	}
	return 0;
}

static struct file_operations device_fops = 
{
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.mmap = device_mmap,
};

static int __init char_device_init(void)
{
#if 0
	int result;
	dev_t dev; //高12位表示主设备号，低20位表示次设备号  
	printk(KERN_ALERT"module init weeds\n");
	printk("dev=%d\n", dev);
	dev = MKDEV(dev_major, dev_minor);
	cdev = cdev_alloc();
	printk(KERN_ALERT"module inti 2\n");
	if (dev_major)
	{
		result = register_chrdev_region(dev, 1, driver_name); //静态分配设备号
		printk("result = %d\n", result);
	}
	else
	{
		result = alloc_chrdev_region(&dev, 0, 1, driver_name);//动态分配设备号
		dev_major = MAJOR(dev);
	}
	
	if (result < 0)
	{
		printk(KERN_WARNING"Can't get major %d\n", dev_major);
		return result;
	}
	
	cdev_init(cdev, &device_fops); //初始化设备cdev
	cdev->ops = &device_fops;
	cdev->owner = THIS_MODULE;
	
	result = cdev_add(cdev, dev, 1);//向内核注册字符设备
#else
	int result;
	result = register_chrdev(107, "fuck", &device_fops);
	if (result)
	{
		printk(KERN_ERR"weeds: failed to register chrdev hwmark (%d)\n", result);
		return result;
	}
	static struct class *mmap_class;
	mmap_class = class_create(THIS_MODULE, "fuck");
	if (IS_ERR(mmap_class))
	{
		result = PTR_ERR(mmap_class);
		printk(KERN_ERR"weeds: failed to class_create fuck (%d)\n", result);
		return result;
	}
	
	device_create(mmap_class, NULL, MKDEV(107, 0), NULL, "fuck");
#endif
	printk(KERN_ALERT"register char device %d\n", result);
	return 0;
}

static void __exit char_device_exit(void)
{
	printk(KERN_ALERT"module exit\n");
	cdev_del(cdev);
	unregister_chrdev_region(MKDEV(dev_major, dev_minor), 1);
}

module_init(char_device_init);
module_exit(char_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard");

