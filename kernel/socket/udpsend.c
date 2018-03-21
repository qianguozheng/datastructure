#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/tcp.h>
#include <net/udp.h>
#include <net/icmp.h>
#include <linux/skbuff.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/inetdevice.h>
#include <linux/in.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <asm/unaligned.h>
#include <linux/kthread.h>

// UDP server sample
// netcat -l -p 8000 -u


MODULE_LICENSE("GPL");
MODULE_AUTHOR("hiweeds");

char *ifname="enp2s0";

//module_param(ifname, charp,0644);
char *buffer="Richard test from kernel\n";

//__u32 dstip = 2818615488; //192.168.0.168
__u32 dstip = 0; //0.0.0.0
__s16 dstport = 8000;
long timeout = 1;

static struct task_struct *kthreadtask=NULL;

static int bind_to_device(struct socket *sock, char *ifname)
{
	struct net *net;
	struct net_device *dev;
	__be32 addr;
	struct sockaddr_in sin;
	int err;
	
	net = sock_net(sock->sk);
	dev = __dev_get_by_name(net, ifname);
	if (!dev){
		printk("No such device named %s\n", ifname);
		return -ENODEV;
	}
	
	addr = inet_select_addr(dev, 0, RT_SCOPE_UNIVERSE);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = addr;
	sin.sin_port = 0;
	err = sock->ops->bind(sock, (struct sockaddr *)&sin, sizeof(sin));
	if (err < 0){
		printk("sock bind err, err=%d\n", err);
		return err;
	}
	return 0;
}

static int connect_to_addr(struct socket *sock)
{
	struct sockaddr_in daddr;
	int err;
	
	daddr.sin_family = AF_INET;
	daddr.sin_addr.s_addr = cpu_to_be32(dstip);
	daddr.sin_port = cpu_to_be16(dstport);
	err = sock->ops->connect(sock, (struct sockaddr *)&daddr, sizeof(struct sockaddr), 0);
	
	if (err < 0){
		printk("sock connect err, err=%d\n", err);
		return err;
	}
	return 0;
}

struct threadinfo{
	struct socket *sock;
	char *buffer;
};

static int sendthread(void *data)
{
	struct kvec iov;
	struct threadinfo *tinfo = data;
	struct msghdr msg = {.msg_flags = MSG_DONTWAIT|MSG_NOSIGNAL};
	int len;
	
	while(!kthread_should_stop()){
		iov.iov_base = (void *)tinfo->buffer;
		iov.iov_len = strlen(tinfo->buffer);
		len = kernel_sendmsg(tinfo->sock, &msg, &iov, 1, strlen(tinfo->buffer));
		if (len != strlen(buffer)){
			printk("kernel_sendmsg err, len=%d, buffer=%d\n", 
				len, (int)strlen(buffer));
			if (len == -ECONNREFUSED){
				printk("Receive Port Unreachable Packet!@\n");
			}
			break;
		}
		schedule_timeout_interruptible(timeout *HZ);
	}
	kthreadtask = NULL;
	sock_release(tinfo->sock);
	kfree(tinfo);
	return 0;
}

static int __init udp_send_init(void){
	int err = 0;
	struct socket *sock;
	struct threadinfo *tinfo;
	
	err = sock_create(PF_INET, SOCK_DGRAM, IPPROTO_UDP, &sock);
	if (err < 0){
		printk("UDP create sock err, err=%d\n", err);
		goto create_error;
	}
	sock->sk->sk_reuse = 1;
	
	err = bind_to_device(sock, ifname);
	if (err < 0){
		printk("Bind to %s err, err=%d\n", ifname, err);
		goto bind_error;
	}
	
	err = connect_to_addr(sock);
	if (err < 0){
		printk("sock connect err, err=%d\n", err);
		goto connect_error;
	}
	
	tinfo = kmalloc(sizeof(struct threadinfo), GFP_KERNEL);
	if (!tinfo){
		printk("kmalloc threadinfo err\n");
		goto kmalloc_error;
	}
	tinfo->sock = sock;
	tinfo->buffer = buffer;
	kthreadtask = kthread_run(sendthread, tinfo, "Hiweeds-sendmsg");
	
	if (IS_ERR(kthreadtask)){
		printk("Create sendmsg thread, err, err=%ld\n", PTR_ERR(kthreadtask));
		goto thread_error;
	}
	printk("UDP send start\n");
	return 0;
	
thread_error:
	kfree(tinfo);
kmalloc_error:
bind_error:
connect_error:
	sock_release(sock);
	kthreadtask = NULL;
create_error:
	return -1;
}

static void __exit udp_send_exit(void){
	if (kthreadtask){
		kthread_stop(kthreadtask);
	}
	printk("UDP send quit\n");
	return;
}

module_init(udp_send_init);
module_exit(udp_send_exit);

