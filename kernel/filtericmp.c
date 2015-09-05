#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/inet.h>

MODULE_LICENSE("GPL");

static struct nf_hook_ops netfilter_ops_in;
static struct nf_hook_ops netfilter_ops_out;
struct sk_buff *sock_buff;
struct iphdr *ip_header;
char *in_iface = "eth0";
char *outface = "eth0";

void log_ip(int sadd,int dadd)
{
    int b1,b2,b3,b4;
    b1 = 255 & sadd;
    b2 = (0xff00 & sadd) >> 8;
    b3 = (0xff0000 & sadd) >> 16;
    b4 = (0xff000000 &sadd) >>24;

    printk("SrcIP: %d.%d.%d.%d",b1,b2,b3,b4);

    b1 = 255 & dadd;
    b2 = (0xff00 & dadd) >> 8;
    b3 = (0xff0000 & dadd) >> 16;
    b4 = (0xff000000 & dadd) >>24;

    printk("  DstIP: %d.%d.%d.%d",b1,b2,b3,b4);
}

unsigned int main_hook(unsigned int hooknum,
                        const struct sk_buff *skb,
                        const struct net_device *in,
                        const struct net_device *out,
                        int(*okfn)(struct sk_buff*))
{
    struct icmphdr* icmp;
    sock_buff = skb_copy(skb,GFP_ATOMIC);
    ip_header = (struct iphdr*)(sock_buff->network_header);
    //ip_header = ip_hdr(sock_buff);

    icmp = (struct icmphdr*) ((char*)ip_header + sizeof(struct iphdr));
    //icmp = icmp_hdr(skb); /* do not return a good value in all cases*/
    log_ip(ip_header->saddr,ip_header->daddr);
    printk("  Dev:%s\n",sock_buff->dev);

    if (icmp->type == ICMP_ECHO)
    {
        printk("ICMP ECHO received and droped\n");
        return NF_DROP;
    }
    return NF_ACCEPT;
}

int init_module(void)
{
    netfilter_ops_in.hook       = main_hook;
    netfilter_ops_in.pf         = PF_INET;
    //netfilter_ops_in.hooknum    = NF_INET_PRE_ROUTING; /*NF_INET_PRE_ROUTING;*/
    netfilter_ops_in.priority   = NF_IP_PRI_FIRST;

    nf_register_hook(&netfilter_ops_in);


    printk(KERN_INFO "sw: init_module() called\n");
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "sw: cleanup_module() called\n");
    nf_unregister_hook(&netfilter_ops_in);
    //nf_unregister_hook(&netfilter_ops_out);
    printk(KERN_INFO "sw: hook unregisted, quit called\n");
}
