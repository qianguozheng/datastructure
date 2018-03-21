#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/route.h>
#include <net/icmp.h>
#include <linux/netfilter_ipv4.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kenthy@163.com");

#define    ETH     "enp2s0"
#define    SIP     "192.168.0.105"
#define    DIP     "192.168.3.137"
#define    SPORT   39804
#define    DPORT   80

unsigned char   SMAC[ETH_ALEN] = {0xf4,0x8e,0x38,0x74,0x19,0x63};
//unsigned char   DMAC[ETH_ALEN] = {0xfa,0x16,0x3e,0xbb,0x86,0xe9};
unsigned char   DMAC[ETH_ALEN] = {0x48,0x4d,0x7e,0xa4,0x05,0x89};

int cp_dev_xmit_tcp (char * eth, u_char * smac, u_char * dmac,
             u_char * pkt, int pkt_len, 
             u_long sip, u_long dip, 
             u_short sport, u_short dport, u_long seq, u_long ack_seq, u_char psh, u_char fin)
{
  struct sk_buff * skb = NULL;
  struct net_device * dev = NULL;
  struct ethhdr * ethdr = NULL;
  struct iphdr * iph = NULL;
  struct tcphdr * tcph = NULL;
  //u_char * pdata = NULL;
  int nret = 1;
  char *p = NULL;
  char app_data[12];

  if (NULL == smac || NULL == dmac) goto out;

  dev = dev_get_by_name(&init_net, eth);
  if (NULL == dev) 
   goto out;

  //skb = alloc_skb (pkt_len + sizeof (struct iphdr) + sizeof (struct tcphdr) + LL_RESERVED_SPACE (dev), GFP_ATOMIC);
  skb = alloc_skb(1500, GFP_ATOMIC);
  
  if (NULL == skb) 
    goto out;
  
  
  memset(app_data, 0, sizeof(app_data));
  memcpy(app_data, "hellowrold", sizeof("hellowrold"));
  printk("app_data=%s\n", app_data);
  
  //skb_reserve (skb, LL_RESERVED_SPACE (dev));
  skb_reserve(skb, 2+sizeof(struct ethhdr) +
					sizeof(struct iphdr) +
					sizeof(struct tcphdr) +
					sizeof(app_data));
	printk("skb_reserved ok\n");
	p = skb_push(skb, sizeof(app_data));
	printk("skb_push app_data ok\n");
	memcpy(p, &app_data[0], sizeof(app_data));
	
	
	
  skb->dev = dev;
  skb->pkt_type = PACKET_OTHERHOST;
  skb->protocol = __constant_htons(ETH_P_IP);
  skb->ip_summed = CHECKSUM_NONE;
  skb->priority = 0;
  
  //pdata = skb_put (skb, pkt_len); 
  //{
   // if (NULL != pkt) 
    // memcpy (pdata, pkt, pkt_len);
  //}
  printk("skb_push tcp header start\n");
	p = skb_push(skb, sizeof(struct tcphdr));
  {
    tcph = (struct tcphdr *) p;
    
    memset (tcph, 0, sizeof (struct tcphdr));
    tcph->source = sport;
    tcph->dest = dport;
    tcph->seq = seq;
    tcph->ack_seq = ack_seq;
    tcph->doff = 5;
    tcph->psh = psh;
    tcph->fin = fin;
    tcph->syn = 1;
    tcph->ack = 0;
    tcph->window = __constant_htons (5840);
    skb->csum = 0;
    tcph->check = 0;
  }
  printk("skb_push tcp header done\n");
  skb_reset_transport_header(skb);
  printk("skb_push ip header start\n");
  
  p = skb_push(skb, sizeof(struct iphdr));
  {
    iph = (struct iphdr*) p;
    iph->version = 4;
    iph->ihl = sizeof(struct iphdr)>>2;
    iph->frag_off = 0;
    iph->protocol = IPPROTO_TCP;
    iph->tos = 0;
    iph->daddr = dip;
    iph->saddr = sip;
    iph->ttl = 0x40;
    iph->tot_len = __constant_htons(skb->len);
    iph->check = 0;
  }
  
  skb->csum = skb_checksum (skb, iph->ihl*4, skb->len - iph->ihl * 4, 0);
  tcph->check = csum_tcpudp_magic (sip, dip, skb->len - iph->ihl * 4, IPPROTO_TCP, skb->csum);
  skb_reset_network_header(skb);
  printk("skb_push ip header done\n");
  p = skb_push (skb, sizeof(struct ethhdr));
  printk("skb_push mac header start\n");
  { 
    ethdr = (struct ethhdr *)p;
    memcpy (ethdr->h_dest, dmac, ETH_ALEN);
    memcpy (ethdr->h_source, smac, ETH_ALEN);
    ethdr->h_proto = __constant_htons (ETH_P_IP);
  }
  skb_reset_mac_header(skb);
  printk("skb_push mac header done\n");


  if (0 > dev_queue_xmit(skb)) {
	  printk("dev_queue_xmit skb failed\n");
	  goto out;
  }
  
  nret = 0;
out:
  if (0 != nret && NULL != skb) {dev_put (dev); kfree_skb (skb);}
  
  return (nret);
}

static int __init init(void)
{
  printk("%s\n","insmod skb_diy module\n");
    
   cp_dev_xmit_tcp (ETH, SMAC, DMAC,NULL, 0, 
                    in_aton(SIP),in_aton(DIP),
                    htons(SPORT),htons(DPORT),
                    0, 0, 0, 0);
	printk("init done\n");
    return 0;
}

static void __exit fini(void)
{
    printk("%s\n","remove skb_diy module.\n");
}

module_init(init);
module_exit(fini);
