#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netdevice.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("geeksword");
MODULE_DESCRIPTION("A Simple Hello Packet Module");

enum {	NF_IP_PRE_ROUTING,
	NF_IP_LOCAL_IN,
	NF_IP_FORWARD,
	NF_IP_LOCAL_OUT,
	NF_IP_POST_ROUTING,
	NF_IP_NUMHOOKS	};

static struct nf_hook_ops in_nfho;   //net filter hook option struct
static struct nf_hook_ops out_nfho;   //net filter hook option struct

static void dump_addr(unsigned char *iphdr)
{
	int i;
	for(i=0; i<4; i++){
		printk("%d.", *(iphdr+12+i));	
	}
	printk(" -> ");
	for(i=0; i<4; i++){
		printk("%d.", *(iphdr+16+i));	
	}
	printk("\n");
}

unsigned int my_hook(unsigned int hooknum,
    struct sk_buff *skb,
    const struct net_device *in,
    const struct net_device *out,
    int (*okfn)(struct sk_buff *))  
{
    printk("Hello packet! ");
    //printk("from %s to %s\n", in->name, out->name);
    unsigned char *iphdr = skb_network_header(skb);
    if(iphdr){
	dump_addr(iphdr);
    }
    return NF_ACCEPT;
}

static int init_filter_if(void)
{
//NF_IP_PRE_ROUTING hook
  in_nfho.hook = my_hook;
  in_nfho.hooknum = NF_IP_LOCAL_IN;
  in_nfho.pf = PF_INET;
  in_nfho.priority = NF_IP_PRI_FIRST;

  nf_register_hook(&in_nfho);

//NF_IP_LOCAL_OUT hook
  out_nfho.hook = my_hook;
  out_nfho.hooknum = NF_IP_LOCAL_OUT;
  out_nfho.pf = PF_INET;
  out_nfho.priority = NF_IP_PRI_FIRST;

  nf_register_hook(&out_nfho);
  return 0;
}

static int hello_init(void)
{
    printk(KERN_INFO "[+] Register Hello_Packet module!\n");
    init_filter_if();
    return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void hello_exit(void)
{
  nf_unregister_hook(&in_nfho);
  nf_unregister_hook(&out_nfho); 
  printk(KERN_INFO "Cleaning up Helllo_Packet module.\n");
}

module_init(hello_init);
module_exit(hello_exit);
