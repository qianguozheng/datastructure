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
	if(argc!=2)  
	{  
		fprintf(stderr,"Usage：%s string\n\a",argv[0]);  
		exit(1);  
	}  
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	if((key=ftok("/tmp/",'m'))==-1)  
	{  
		fprintf(stderr,"Creat Key Error：%s\a\n",strerror(errno));  
		exit(1);  
	}  
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	if((msgid=msgget(key,PERM))==-1)  
	{  
		fprintf(stderr,"Creat Message Error：%s\a\n",strerror(errno));  
		exit(1);  
	}  
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	msg.mtype=10;  
	strncpy(msg.buffer,argv[1],BUFFER);  
	printf("%s(%d): msg.buffer=%s\n", __FUNCTION__, __LINE__, msg.buffer);
	msgsnd(msgid,&msg,sizeof(struct msgtype),0);
	printf("%s(%d)\n", __FUNCTION__, __LINE__);  
	memset(&msg,'\0',sizeof(struct msgtype));  
	printf("%s(%d)\n", __FUNCTION__, __LINE__);
	msgrcv(msgid,&msg,sizeof(struct msgtype),msg.mtype,0);  
	fprintf(stderr,"Client receive：%s\n",msg.buffer);  
	exit(0);  
}
