#include <string.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/msg.h>  
#include <sys/stat.h>
#include <stdio.h>
#define MSG_FILE "server.c"  
#define BUFFER 255  
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
	
	if(argc!=2)  
	{  
		fprintf(stderr,"Usage：%s string\n\a",argv[0]);  
		exit(1);  
	}  

	if((key=ftok("/tinyproxy/",'C'))==-1)  
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

	rc = msgsnd(msgid,&msg,sizeof(struct msgtype),0);
	if (rc < 0)
	{
	   printf("%s(%d): rc=%d\n", __FUNCTION__, __LINE__, rc);
	}
	//memset(&msg,'\0',sizeof(struct msgtype));  
	//msgrcv(msgid,&msg,sizeof(struct msgtype),msg.mtype,0);
	/*printf("Receive messge=%s\n", msg.buffer);
	if (rc < 0)
	{
	   printf("%s(%d): rc=%d\n", __FUNCTION__, __LINE__, rc);
	} 
	
	// remove message queue
	rc = msgctl(msgid, IPC_RMID, NULL);
	if (rc < 0)
	{
	   printf("%s(%d): rc=%d\n", __FUNCTION__, __LINE__, rc);
	}*/
	
	return 0;
}
