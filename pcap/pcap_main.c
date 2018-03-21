#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <netinet/ether.h>
#include <signal.h>
#include <getopt.h>
int loop = 1;
pcap_t *descr = NULL;
pcap_dumper_t *dump = NULL;

u_char *handle_IP(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet);

u_int16_t handle_ethernet(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet){
	struct ether_header *eptr;
	eptr = (struct ether_header *) packet;
	//fprintf(stdout, "ethernet header source: %s ", ether_ntoa(eptr->ether_shost));
	//fprintf(stdout, "ethernet header destination: %s ", ether_ntoa(eptr->ether_dhost));
	
	//Save to dump file
	if (dump) {
		pcap_dump((char *)dump, pkthdr, packet);
	}
	
	if (ntohs(eptr->ether_type) == ETHERTYPE_IP){
		fprintf(stdout, "(IP)");
		fprintf(stdout, "\n");
		handle_IP(args, pkthdr, packet);
	} else if (ntohs(eptr->ether_type) == ETHERTYPE_ARP){
		fprintf(stdout, "(ARP)");
	} else if (ntohs(eptr->ether_type) == ETHERTYPE_REVARP){
		fprintf(stdout, "(RARP)");
	} else {
		fprintf(stdout, "?\n");
		//exit(1);
		return 0;
	}
	fprintf(stdout, "\n");
	return eptr->ether_type;
}

struct my_ip {
	u_int8_t ip_vhl;
#define IP_V(ip) (((ip)->ip_vhl & 0xf0) >> 4)
#define IP_HL(ip) ((ip)->ip_vhl & 0x0f)
	u_int8_t ip_tos;
	u_int8_t ip_len;
	u_int8_t ip_id;
	u_int8_t ip_off;
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
	u_int8_t ip_ttl;
	u_int8_t ip_p;
	u_int16_t ip_sum;
	struct in_addr ip_src, ip_dst;
};
u_char *handle_IP(u_char *args, const struct pcap_pkthdr *pkthdr, const u_char *packet){
	const struct my_ip *ip;
	u_int length = pkthdr->len;
	u_int hlen, off, version;
	int i;
	
	int len;
	
	ip = (struct my_ip *)(packet + sizeof(struct ether_header));
	length -= sizeof(struct ether_header);
	
	if (length < sizeof(struct my_ip)){
		printf("truncated ip %d", length);
		return NULL;
	}
	
	len = ntohs(ip->ip_len);
	hlen = IP_HL(ip);
	version = IP_V(ip);
	
	if (version != 4){
		fprintf(stdout, "Unknown version %d\n", version);
		return NULL;
	}
	
	if (hlen < 5){
		fprintf(stdout, "bad-hlen %d\n", hlen);
	}
	
	if (length < len) {
		printf("\ntruncated IP - %d bytes missing\n", len -length);
	}
	
	off = ntohs(ip->ip_off);
	if ((off & 0x1fff) == 0){
		fprintf(stdout, "IP: ");
		fprintf(stdout, "%s ", inet_ntoa(ip->ip_src));
		fprintf(stdout, "%s %d %d %d %d", inet_ntoa(ip->ip_dst),
				hlen, version, len, off);
	}
	
	return NULL;
}

/*
void my_callback(u_char *args, const struct pcap_pkthdr * pkthdr, u_char * packet){
	u_int16_t type = handle_ethernet(args, pkthdr, packet);
	
	if (type == ETHERTYPE_IP){
		handle_IP(args, pkthdr, packet);
	} else if (type == ETHERTYPE_ARP){
		
	} else if (type == ETHERTYPE_REVARP){
		
	}
}*/

void statistics(){
	struct pcap_stat ps;
	pcap_stats(descr, &ps);
	fprintf(stdout, "Received %d, Drop %d, Drop by Interface %d\n", 
		ps.ps_recv, ps.ps_drop, ps.ps_ifdrop);
}

void sig_handle(int sig)
{
	 if (SIGINT == sig) {
		  loop = 0;
		  printf("\n- END -\n");
		  statistics();
		  
		  pcap_breakloop(descr);
	 }
}

char dumpname[256];
char filter[256];
int packets = -1;

void parseOptions(int argc, char **argv){
	int ch;
	opterr = 0;
	while((ch = getopt(argc, argv, "w:f::p::h")) != -1){
		switch(ch){
			case 'w':
				memset(dumpname, 0, sizeof(dumpname));
				printf("filename=%s\n", optarg);
				sprintf(dumpname,"%s", optarg);
				break;
			case 'f':
				memset(filter, 0, sizeof(filter));
				printf("filter=%s\n", optarg);
				sprintf(filter,"%s", optarg);
			case 'p':
				
				printf("optarg=%s\n", optarg);
				packets = atoi(optarg);
				break;
			case 'h':
				printf("Usage: ");
				break;
			default:
				break;
		}
	}
}
int main(int argc, char **argv){
	char *dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	
	struct bpf_program fp;
	bpf_u_int32 maskp;
	bpf_u_int32 netp;
	u_char * args = NULL;
	
	if (argc < 2){
		fprintf(stdout, "Usage: %s numpackets \"options\"\n", argv[0]);
		return 0;
	}
	
	memset(filter, 0, sizeof(filter));
	memset(dumpname, 0, sizeof(dumpname));
	
	parseOptions(argc, argv);
	
	signal(SIGINT, sig_handle);//register ctrl+c to system
	
	dev = pcap_lookupdev(errbuf);
	if (NULL == dev){
		printf("%s\n", errbuf);
		exit(1);
	}
	
	pcap_lookupnet(dev, &netp, &maskp, errbuf);
	
	printf("dev = %s\n", dev);
	descr = pcap_open_live(dev, BUFSIZ, 1, -1, errbuf);
	if (NULL == descr){
		printf("pcap_open_live(): %s\n", errbuf);
		exit(1);
	}
	
	//if (argc > 2){
		if (strlen(filter) > 0)
		{
			if (pcap_compile(descr, &fp, argv[2], 0, netp) == NULL){
				fprintf(stderr, "Error calling pcap_compile\n");
				exit(1);
			}
			
			if (pcap_setfilter(descr, &fp) == -1){
				fprintf(stderr, "Error setting filter\n");
				exit(1);
			}
		}
		//if (argc >= 4){
	fprintf(stdout, "save pcap to file %s\n", dumpname);
	if (strlen(dumpname) > 0){
		dump = pcap_dump_open(descr, dumpname);
		if (NULL == dump){
			fprintf(stderr, "open dump file failed\n");
			exit(1);
		}
	}
	
	do {
		pcap_loop(descr, packets, handle_ethernet, args);
	}while(loop);
	
	//Save pcap file
	if (dump)
		pcap_dump_close(dump);
	
	fprintf(stdout, "\nfinished\n");
	return 0;
}



