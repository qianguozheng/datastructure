#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/msg.h>  
#include <sys/stat.h>
#include <stdio.h>
#define MSG_FILE "server.c"  
#define BUFFER 2048  
#define PERM S_IRUSR|S_IWUSR  
struct msgtype {  
	long mtype;  
	char buffer[BUFFER+1];  
};  
int main(int argc,char **argv)  
{  
	struct msgtype msg;  
	key_t key;  
	int msgid;  
	int rc = 0;
	char urlstr[2048];
	char ipaddr[128];
	time_t active_time;
	
	if(argc!=2)  
	{  
		fprintf(stderr,"Usage：%s string\n\a",argv[0]);  
		exit(1);  
	}  

	if((key=ftok("/etc/",'C'))==-1)  
	{  
		fprintf(stderr,"Creat Key Error：%s\a\n",strerror(errno));  
		exit(1);  
	}  

	if((msgid=msgget(key,0660 | IPC_CREAT))==-1)  
	{  
		fprintf(stderr,"Creat Message Error：%s\a\n",strerror(errno));  
		exit(1);  
	}  

	msg.mtype=10;  
	strncpy(msg.buffer,argv[1],BUFFER);  

	//rc = msgsnd(msgid,&msg,sizeof(struct msgtype),0);
	//if (rc < 0)
	//{
	//   printf("%s(%d): rc=%d\n", __FUNCTION__, __LINE__, rc);
	//}
	while(1)
	{
		memset(&msg,'\0',sizeof(struct msgtype));  
		memset(urlstr, 0, sizeof(urlstr));
		memset(ipaddr, 0, sizeof(ipaddr));
		
		if ((rc = msgrcv(msgid,&msg,sizeof(struct msgtype),msg.mtype,0)) >= 0)
		{
			printf("Receive messge=%s\n", msg.buffer);
		}
		
		if (rc < 0)
		{
			perror("rcv error");
		   printf("%s(%d): rc=%d\n", __FUNCTION__, __LINE__, rc);
		}
		if (3 == sscanf(msg.buffer, "%s %s %lu", ipaddr, urlstr, &active_time))
		{
			printf("ip=%s\n", ipaddr);
			printf("urlstr=%s\n", urlstr);
			printf("active=%lu\n", active_time);
			
			
		}
		else
		{
			printf("Parse message failed\n");
		}
		
	}	
	// remove message queue
	rc = msgctl(msgid, IPC_RMID, NULL);
	if (rc < 0)
	{
	   printf("%s(%d): rc=%d\n", __FUNCTION__, __LINE__, rc);
	}
	
	return 0;
}
