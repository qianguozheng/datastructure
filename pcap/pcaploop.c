#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pcap.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define IP_Swap_32(x) ( ( ((x)>>24) | (((x)>>8)&0xff00) ) | ( ((x)<<24) | (((x)<<8)&0xff0000) ) )
#define Max_Cap_Loop 100
#define MAXINTERFACES 16
unsigned char Count=0; //累计抓包次数
unsigned long Packet=0; //每次抓包个数
unsigned long TotalCap=0; //总包数
unsigned long TotalDrop=0;//总丢弃数
pthread_t Time; //计时，每次抓30秒
pthread_t Loop_Packet; //循环抓包
pthread_mutex_t mut; //临界区信号量,到指定时间后Count自增，并重新启动抓包线程
char Filter[8096];
unsigned long CapTime=0;
pcap_if_t *alldevs;
struct pcap_stat p_s; //状态
pcap_t *adhandle;  //运行时句柄
char errbuf[PCAP_ERRBUF_SIZE];
struct bpf_program fp;
bpf_u_int32 maskp;
bpf_u_int32 netp;
struct InterfaceInfo{
 char *in_name;
 char *description;
 unsigned char ip[4];
 unsigned char mask[4];
};
struct Turple{
 unsigned char proto;
 unsigned char sip[4];
 unsigned char dip[4];
 unsigned short sport;
 unsigned short dport; 
} turple;
struct InterfaceInfo net_interface[MAXINTERFACES];
int count_interface=0;
int timeup=0;
void sig_handle(int sig)
{
	 if (SIGINT == sig) {
		  timeup=-1;
		  printf("\n- END -\n");
	 }
}
void callback(u_char *param,const struct pcap_pkthdr *header,const u_char *pkt_data)
{
	
	 char timestr[16];
	 //tm *ltime=localtime(&header->ts.tv_sec);
	 time_t dtime=header->ts.tv_sec;
	 u_int len=header->len;
	 u_int caplen=header->caplen;
	 printf("Time:%s Len:%4d,Captured:%4d  pthread_id:%u ====\t",ctime(&dtime),len,caplen, pthread_self());
	 //for(int i=0;i<header->caplen;i++)
	  //printf("%02x",pkt_data[i]);//return;
	 if(pkt_data[12]==0x08 && pkt_data[13]==0x00)
	 {
		turple.proto=pkt_data[23];
		memcpy(turple.sip,(const void*)&pkt_data[26],8);
		memcpy(&turple.sport,(const void *)&pkt_data[34],4);
		turple.sport=turple.sport>>8|turple.sport<<8;
		turple.dport=turple.dport>>8|turple.dport<<8;
		printf("%d.%d.%d.%d:%d->%d.%d.%d.%d:%d|%d\n",
			turple.sip[0],turple.sip[1],turple.sip[2],turple.sip[3],turple.sport,
			turple.dip[0],turple.dip[1],turple.dip[2],turple.dip[3],turple.dport,turple.proto);
	 }
 //printf("\n");
}
void CountTime(void)
{
	 for(;;)
	 {
		sleep(CapTime);
		Count++;
		pthread_mutex_lock(&mut);
		timeup=1;
		pthread_mutex_unlock(&mut);
	 }
}
int InitCap(char *filterstring)
{
	 if(pcap_findalldevs(&alldevs,errbuf)== -1)
	 {
		  pcap_freealldevs(alldevs);
		  fprintf(stderr,"Error in pcap_findalldevs:%s\n",errbuf);
		  return -1;
	 }
	 pcap_if_t *d;
	 int iCount=0;
	 for(d=alldevs;d;d=d->next)
	 {
		  net_interface[count_interface].in_name=(char *)malloc(strlen(d->name)+1);
		  memcpy(net_interface[count_interface].in_name,d->name,strlen(d->name)+1);
		  if(d->description)
		  {
			net_interface[count_interface].description=(char *)malloc(strlen(d->description)+1);
			memcpy(net_interface[count_interface].description,d->description,strlen(d->description)+1);
		  }
		  else
			net_interface[count_interface].description="No description";  
		  memset(net_interface[count_interface].ip,0,4);
		  pcap_lookupnet(d->name,&netp,&maskp,errbuf);
		  net_interface[count_interface].mask[0]=maskp>>24;
		  net_interface[count_interface].mask[1]=maskp>>16;
		  net_interface[count_interface].mask[2]=maskp>>8;
		  net_interface[count_interface].mask[3]=maskp;
		  
		  count_interface++;
		  iCount++;
	 }
	 
	 if(iCount==0)
	 {
		  pcap_freealldevs(alldevs);
		  printf("\nNo interface found! Make sure Libpcap is working");
		  return -2;
	 }
	 pcap_freealldevs(alldevs);
	 return 0;
}
int InitCap_m(void)
{
	register int fd, interface;
	struct ifreq buf[MAXINTERFACES];
	struct ifconf ifc;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof buf;
		ifc.ifc_buf = (caddr_t) buf;
		if (!ioctl(fd, SIOCGIFCONF, (char *) &ifc))
		{
			interface = ifc.ifc_len / sizeof(struct ifreq);
				 //printf("interface num is interface=%d\n\n", interface);
			while (interface-- > 0)
			{
				for(int i=0;i<count_interface;i++)//用该接口名对比pcap获取的接口名,若一致则置IP(因为或者的次序可能不同所以全比较)
				if((strcmp(buf[interface].ifr_name,net_interface[i].in_name))==0)
				{
					if (!(ioctl(fd, SIOCGIFADDR, (char *) &buf[interface])))
					{
						unsigned long v;
						v=*((unsigned long*)&(((struct sockaddr_in*)(&buf[interface].ifr_addr))->sin_addr));
						memcpy(net_interface[i].ip,(unsigned char *)(&v),4);
					}
					else
					{
						char str[256] = "";
						sprintf(str, "cpm: ioctl device %s",
						buf[interface].ifr_name);
						perror(str);
						close(fd);
						return -1;
					}
				}
			}//end of while
		}
		else
		{
			perror("cpm: ioctl");
			close(fd);
			return -1;
		}
	}
	else
	{
		perror("cpm: socket");
		return -1;
	}
	close(fd);
	return 0;
}
int show_choose_dev(void)
{
	int iChoose;
	int iCount=count_interface;
	for(int i=0;i<count_interface;i++)
	{
		printf("%d.%s(%s) ip_addr:%d.%d.%d.%d,netmask:%d.%d.%d.%d\n",
		i+1,net_interface[i].in_name,net_interface[i].description,
		net_interface[i].ip[0],net_interface[i].ip[1],net_interface[i].ip[2],net_interface[i].ip[3],
		net_interface[i].mask[0],net_interface[i].mask[1],net_interface[i].mask[2],net_interface[i].mask[3]
		);
	}
	printf("Choose an interface(1-%d):",iCount);
	scanf("%d",&iChoose);
	printf("Your Choose:%d\n",iChoose);
	if(iChoose<1||iChoose>iCount)
	{
		printf("\nInterface number out of range.\n");
		return -1;
	}
	printf("\nStart to sniffer on %s..\n",net_interface[iChoose-1].in_name);
	return iChoose; 
}
int StartCap(int dev_num)
{
	int iChoose=dev_num;
	adhandle=pcap_open_live(net_interface[iChoose-1].in_name,65535,1,1000,errbuf);
	if(adhandle==NULL)
	{
		fprintf(stderr,"\nUnable to open the adapter.\n");
		return -1;  
	}
	maskp=(bpf_u_int32)(IP_Swap_32(*((unsigned long*)(&(net_interface[iChoose-1].mask)))));
	if(pcap_compile(adhandle,&fp,Filter,0,maskp)==-1)
	{
		fprintf(stderr,"Error calling pcap_compile\n");
		pcap_close(adhandle);
		return -1;
	}
	if(pcap_setfilter(adhandle,&fp)==-1)
	{
		pcap_close(adhandle);
		fprintf(stderr,"Error setting filter\n");
		return -1;
	}
	pcap_loop(adhandle,1000000,callback,NULL);
	//printf("close========================================================================\n");
	//pcap_close(adhandle);//pcap_breakloop函数会关闭句柄故再次关闭会导致重复释放
	return 0;
}
int main(int argc,char *argv[])
{
	 int create_thread_retval;
	 if(argc<=1 ||argc>3)
	 {
		  printf("\t Usage:%s cap_time(seconds) \"filter\"\n",argv[0]);
		  return -1;
	 }
	 memset(Filter,0,8096);
	 memcpy(Filter,argv[2],strlen(argv[2]));
	 CapTime=atol(argv[1]);
	 //printf("%d,%s,%d\n",argc,Filter,CapTime);
	 signal(SIGINT, sig_handle);//register ctrl+c to system
	 if(CapTime<=0||CapTime>0xffffff)
	 {
		  printf("Please input acceptable time\n");
		  return -1;
	 }
	 //printf("-\n");
	 if(InitCap(Filter)!=0)
	 {
		  printf("Initiation capture failed!\n");
		  return -1;
	 }
	 if(InitCap_m()!=0)
	 {
		  printf("Get network interface failed!\n");
		  return -1;
	 }
	 int dev_num=show_choose_dev();
	 printf("main process starting:\n");
	 if((create_thread_retval=pthread_create(&Time,NULL,(void*(*)(void*))CountTime,NULL))!=0)
	 {
		  printf("Create 'Count_Time' thread failed!\n");
		  return -1;
	 }
	 if((create_thread_retval=pthread_create(&Loop_Packet,NULL,(void*(*)(void*))StartCap,(void*)dev_num))!=0)
	 {
		  printf("Create 'Packet_Capture' thread failed!\n");
		  return -1;
	 }
	 
	//if((create_thread_retval=pthread_create(&Loop_Packet,NULL,(void*(*)(void*))StartCap,(void*)dev_num))!=0)
	//{
	//	printf("Create 'Packet_Capture' thread failed!\n");
	//	return -1;
	//}
	 
	 while(timeup!=-1)
	 {
		while(timeup==1)
		{
			pthread_mutex_lock(&mut);
			printf("_%d# capture finished==|++",Count);
			timeup=0;
			pcap_stats(adhandle,&p_s);
			TotalCap+=p_s.ps_recv,
			TotalDrop+=p_s.ps_drop;
			printf("recv:%u,drop:%u,T_Cap=%u,T_Drop=%u\n\n",p_s.ps_recv,p_s.ps_drop,TotalCap,TotalDrop);
			//pcap_breakloop(adhandle);
			//if((create_thread_retval=pthread_create(&Loop_Packet,NULL,(void*(*)(void*))StartCap,(void*)dev_num))!=0)
			//{
			//	printf("Create 'Packet_Capture' thread failed!\n");
			//	return -1;
			//}
			pthread_mutex_unlock(&mut);
		}
	 }
	 return 0;
}
