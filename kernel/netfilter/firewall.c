#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <linux/proc_fs.h>
#include <linux/icmp.h>
#include <linux/netdevice.h>
#include <linux/string.h>

/*
 *  Defining the IPs in integer
 * ----
 * Topology uses the following IP address:
 * 
 * Network in which hosts are present: 10.0.0.0/24
 * WebServer IP: 10.0.0.100 (link2)
 * Remote Client: 192.168.1.0/24 (link5)
 * 
 */
 
#define HOST_NW_IP 167772160
#define HOST_NW_IP_prefix 24

#define WEBSERVER_IP 167772260
#define WEBSERVER_IP_prefix 32

#define REMOTE_CLIENT 3232235776
#define REMOTE_CLIENT_prefix 24

static struct nf_hook_ops netfilter_ops_in;

/*
A function "network_has_ip()" is defined below which is used to 
* find out if a given IP address is present in a given IP or not.
* 
* The function returns 0 if function value is false and 1 if true.
*/

bool network_has_ip(unsigned int ip, unsigned int nw_ip, unsigned int prefix)
{
	unsigned int mask = ~(0xffffffff >> prefix);
	unsigned int mask_1, mask_2;
	
	mask_1 = nw_ip & mask;
	mask_2 = ip & mask;
	
	return mask_1 == mask_2;
}

// Main loop
unsigned int main_hook(unsigned int hooknum, struct sk_buff *skb, const struct
						net_device *in, const struct net_device *out, 
						int (*okfn)(struct sk_buff *))
{
	//Define structures for corresponding headers
	struct iphdr *ip_header = (struct iphdr *)ip_hdr(skb);
	struct icmphdr *icmp_header;
	struct tcphdr *tcp_header;
	
	unsigned int src_ip = (unsigned int)ip_header->saddr;
	unsigned int dest_ip = (unsigned int)ip_header->daddr;
	unsigned int icmp_type = 0;
	unsigned int src_port = 0;
	unsigned int dest_port = 0;
	
	char *management = "eth0";
	if (strcmp(in->name, management) == 0){
		return NF_ACCEPT;
	}
	
	if (ip_header->protocol == 1){
		icmp_header = (struct icmphdr *)(ip_hdr(skb));
		icmp_type = icmp_header->type;
	}
	else if (ip_header->protocol == 6)
	{
		tcp_header = (struct tcphdr *)(tcp_hdr(skb));
		src_port = ntohs((unsigned int) tcp_header->source);
		dest_port = ntohs((unsigned int) tcp_header->dest);
	}
	
	//Debug information
	printk("Packet Information: icmp type: %u,\n", icmp_type);
	printk("Packet Information: interface: %ph, "
		"src ip: %u (%pI4),"
		"src port: %u; dest ip:%u (%pI4),"
		"dest port: %u; proto %u\n",
		in->name, src_ip, &src_ip, src_port, 
		dest_ip, &dest_ip, dest_port,
		ip_header->protocol
	);
	
	if (1 == ip_header->protocol)
	{
		if (network_has_ip(ntohl(src_ip), REMOTE_CLIENT, 24)){
			if (icmp_hdr(skb)->type == 0){
				printk("icmp\n");
				return NF_ACCEPT;
			}
		}
		if ((icmp_hdr(skb)->type == 8) && !network_has_ip((ntohl(dest_ip)),WEBSERVER_IP, WEBSERVER_IP_prefix)){
			printk(KERN_INFO"Block Rule 1: A remote client with IP (%pI4) at %s is sending an ICMP packet to the web server %pI4\n", &src_ip, in->name, &dest_ip);
			return NF_DROP;
		}
	}
	pr_info("interface=[%p0] [%po]\n", out->name, out->name);
	return NF_ACCEPT;
}

//Init Module

int init_module(void){
	printk(KERN_INFO"Initializing for firewall kernel module\n");
	netfilter_ops_in.hook = main_hook;
	netfilter_ops_in.hooknum = NF_INET_LOCAL_IN;//NF_INET_PRE_ROUTING;
	netfilter_ops_in.pf = PF_INET;
	netfilter_ops_in.priority = NF_IP_PRI_FIRST;
	nf_register_hook(&netfilter_ops_in);
	return 0;
}

void cleanup_module(void){
	nf_unregister_hook(&netfilter_ops_in);
	printk(KERN_INFO"Firewall kernel module unloaded.\n");
}


