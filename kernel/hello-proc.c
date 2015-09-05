#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
static char *buffer = NULL;

static int hello_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello proc!\n");
  seq_printf(m, "buffer=%s\n", buffer);
  int i = 0;
  
  if (buffer != NULL)
  {  
    if (buffer[0] == 's')
    {
      for (i = 0; i < 10; i++)
        seq_printf(m, "i = %d\n", i);
    }
  }
  return 0;
}

static ssize_t hello_proc_write(struct file *file, const char *buffer, size_t len,
  loff_t *off)
{
  int user_len = 0;
  user_len = len;
  buffer = (char *)kmalloc(user_len+1, GFP_KERNEL);
  memset(buffer, 0, user_len + 1);
  if (copy_from_user(buffer, buffer, user_len))
  {
    printk( "hello_proc error\n");
    return -EFAULT;
  }
  printk( "userlen=%d\n", user_len);
  return user_len;
}

static int hello_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, hello_proc_show, NULL);
}
  
static const struct file_operations hello_proc_fops = {
  .owner = THIS_MODULE,
  .open = hello_proc_open,
  .read = seq_read,
  .write = hello_proc_write,
  .llseek = seq_lseek,
  .release = single_release,
};

static int __init hello_proc_init(void) {
  proc_create("hello_proc", 0666, NULL, &hello_proc_fops);
  return 0;
}

static void __exit hello_proc_exit(void) {
  remove_proc_entry("hello_proc", NULL);
}

MODULE_LICENSE("GPL");
module_init(hello_proc_init);
module_exit(hello_proc_exit);
