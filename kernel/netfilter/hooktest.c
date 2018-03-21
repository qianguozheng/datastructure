#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <linux/netfilter_ipv4.h>
#include <linux/inet.h>

#define MAC_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_ARG(x) ((u8*)(x))[0],((u8*)(x))[1],((u8*)(x))[2],((u8*)(x))[3],((u8*)(x))[4],((u8*)(x))[5]

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kenthy@163.com");

const char* hooks[] ={ "NF_IP_PRE_ROUTING",
                             "NF_IP_LOCAL_IN",
                             "NF_IP_FORWARD",
                             "NF_IP_LOCAL_OUT",
                             "NF_IP_POST_ROUTING"};

void print_ipproto(int proto)
{
switch(proto)
{
        case IPPROTO_ICMP:
                printk("\t%s\n", "IPPROTO_ICMP");
          break;
        case IPPROTO_TCP:
                printk("\t%s\n", "IPPROTO_TCP");
          break;
        case IPPROTO_UDP:
                printk("\t%s\n", "IPPROTO_UDP");
          break;
        default:
                printk("\t%s\n", "other IPPROTO");
        }
}

void print_mac(struct ethhdr* eth)
{
if(eth==NULL)
        return;
        
if(eth->h_source!=NULL)
        printk("SOURCE:" MAC_FMT "\n", MAC_ARG(eth->h_source));

if(eth->h_dest!=NULL)
             printk("DEST:" MAC_FMT "\n", MAC_ARG(eth->h_dest));
}

//typedef unsigned int nf_hookfn(void *priv,
//			       struct sk_buff *skb,
//			       const struct nf_hook_state *state);


unsigned int
mac(void *priv, 
	struct sk_buff* skb,
	const struct nf_hook_state *state)
{
   struct sk_buff* nskb;
   struct iphdr *iph = NULL;
   struct ethhdr* eth;
  
  //printk("hooknum=%u\n", state->hook);
  if (state->hook > 5){
		printk("hooknum invalid\n");
		return NF_ACCEPT;
	}
  nskb = skb;
  if(nskb==NULL)
  {
    printk("%s\n", "*skb is NULL\n");
    return NF_ACCEPT;
   }
  
	iph = ip_hdr(nskb);
	if(iph == NULL)
	{
		printk("%s\n", "*iph is NULL\n");
		return NF_ACCEPT;
	}

      
   printk("------begin %s--------\n", hooks[state->hook]);
   print_ipproto(iph->protocol);
   printk("%pI4 -> %pI4\n", &iph->saddr, &iph->daddr);
   //printk("len is %d, data len is %d\n", nskb->len, nskb->data_len);
   printk("len=%d,mac_len=%d\n", nskb->len, nskb->mac_len);
   
	if (state->hook == NF_INET_LOCAL_IN) {
	   if (iph->saddr == in_aton("112.74.112.103")) {
			printk("tcp request to hiweeds.net\n");
			unsigned char insert[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'}; //插入数据
			unsigned int data_len = 0;
			unsigned char *tcp_ins = NULL;
			unsigned int tcp_len = 0;
			struct tcphdr *tcph = NULL;
#if 0
			printk("TCP tail: 0x%u\n",skb->tail);                
			skb_put(skb,16); //数据段空间扩展
			printk("TCP new_tail: 0x%u\n",skb->tail);
			printk("TCP------tailroom: %u------\n",skb_tailroom(skb));
			tcph = tcp_hdr(skb);
			tcp_ins = (unsigned char *)tcph + (tcph->doff*4); //指向数据起始处
			iph->tot_len = htons(ntohs(iph->tot_len)+16);
			tcp_len = ntohs(iph->tot_len) - iph->ihl*4;
			data_len = tcp_len - (tcph->doff*4);
			memmove(tcp_ins+16, tcp_ins, data_len); //原始数据后移留出插入空间
			memcpy(tcp_ins, insert, 16); //拷贝插入数据
			tcph->check = 0;
			skb->csum = 0;
			iph->check = 0;
			skb->csum = skb_checksum(skb, iph->ihl*4,ntohs(iph->tot_len)-iph->ihl*4,0);
			tcph->check = csum_tcpudp_magic(iph->saddr, iph->daddr, tcp_len, iph->protocol, skb->csum);
			iph->check = ip_fast_csum(iph, iph->ihl);
#endif

		}
	}
   if(nskb->mac_len > 0)
   {
            eth = (struct ethhdr*)skb_mac_header(skb);
            print_mac(eth);        
   }
   else{
		//printk("%s", "mac is NULL\n");                
    }
    
   printk("------end  %s--------\n\n", hooks[state->hook]);
  
    return NF_ACCEPT;
}

                             
static struct nf_hook_ops mac_ops[] = {
        {
                .hook                = mac,
                //.owner                = THIS_MODULE,
                .pf                = PF_INET,
                .hooknum        = NF_INET_PRE_ROUTING,
                .priority = NF_IP_PRI_FIRST,
        },
        {
                .hook                = mac,
                //.owner                = THIS_MODULE,
                .pf                = PF_INET,
                .hooknum        = NF_INET_LOCAL_IN,
                .priority = NF_IP_PRI_FIRST,
        },
        /*{
                .hook                = mac,
                //.owner                = THIS_MODULE,
                .pf                = PF_INET,
                .hooknum        = NF_INET_FORWARD,
                .priority = NF_IP_PRI_FIRST,
        },
        {
                .hook                = mac,
                //.owner                = THIS_MODULE,
                .pf                = PF_INET,
                .hooknum        = NF_INET_LOCAL_OUT,
                .priority = NF_IP_PRI_FIRST,
        },
        {
                .hook                = mac,
                //.owner                = THIS_MODULE,
                .pf                = PF_INET,
                .hooknum        = NF_INET_POST_ROUTING,
                .priority = NF_IP_PRI_FIRST,
        },*/
};

static int __init init(void)
{
    int ret;
    ret = nf_register_hooks(mac_ops, ARRAY_SIZE(mac_ops));
    if (ret < 0) {
        printk("http detect:can't register mac_ops detect hook!\n");
        return ret;
    }
    printk("insmod mac_ops detect module\n");
    return 0;
}

static void __exit fini(void)
{
    nf_unregister_hooks(mac_ops, ARRAY_SIZE(mac_ops));
    printk("remove mac_ops detect module.\n");
}

module_init(init);
module_exit(fini);

