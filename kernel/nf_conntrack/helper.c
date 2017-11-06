#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/tcp.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_helper.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Richard");
MODULE_DESCRIPTION("expect helper test");

///http://blog.csdn.net/wuwenhuahua/article/details/42425881
struct aa_proto {
		int type;
		int port;
		union nf_inet_addr addr;
};

static int aa_help (struct sk_buff *skb,
			unsigned int protoff,
			struct nf_conn *ct,
			enum ip_conntrack_info ctinfo)
{
	unsigned int dataoff, datalen;
	const struct tcphdr *th;
	struct tcphdr _tcph;
	int ret;
	char *dt_ptr;
	struct nf_conntrack_expect *exp;
	
	int dir = CTINFO2DIR(ctinfo);
	struct aa_proto prot = {0};
	uint16_t port = ntohs((u_int16_t) prot.port);
	char aa_buffer[512];
	
	if (ctinfo != IP_CT_ESTABLISHED
		&& ctinfo != IP_CT_ESTABLISHED+IP_CT_IS_REPLY){
		return NF_ACCEPT;	
	}
	
	//parse packet content
	th = skb_header_pointer(skb, protoff, sizeof(_tcph), &_tcph);
	dataoff = protoff + th->doff * 4;
	datalen = skb->len - dataoff;
	dt_ptr = skb_header_pointer(skb, dataoff, datalen, aa_buffer);
	
	// copy protocol header
	memcpy(&prot, dt_ptr, sizeof(struct aa_proto));
	if (prot.type != 12) { //if not predefined type 12,then do not need expect connection
		ret = NF_ACCEPT;
		goto out;
	}
	
	exp = nf_ct_expect_alloc(ct);
	port = ntohs((uint16_t) prot.port);
	nf_ct_expect_init(exp, NF_CT_EXPECT_CLASS_DEFAULT, AF_INET,
					&ct->tuplehash[dir].tuple.src.u3, &prot.addr,
					IPPROTO_TCP, NULL, &port);
	if (nf_ct_expect_related(exp) != 0) {
		ret = NF_DROP;
	} else {
		ret = NF_ACCEPT;
	}
out:
	return ret;
}

static const struct nf_conntrack_expect_policy aa_policy = {
	.max_expected = 10,
	.timeout = 50*60,
};

static struct nf_conntrack_helper aa = {
		.name = "aa",
		.me = THIS_MODULE,
		.tuple.src.u.tcp.port = cpu_to_be16(12345),
		.tuple.dst.protonum = IPPROTO_TCP,
		.help = aa_help,
		.expect_policy = &aa_policy,
};

static void nf_conntrack_aa_fini(void){
		nf_conntrack_helper_unregister(&aa);
}
static int __init nf_conntrack_aa_init(void){
		int ret = nf_conntrack_helper_register(&aa);
		if (ret) {
				nf_conntrack_aa_fini();
		}
		return ret;
}

module_init(nf_conntrack_aa_init);
module_exit(nf_conntrack_aa_fini);
