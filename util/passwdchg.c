#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int execute_cmd()
{
	int fd[2];
	int fd2[2];
	pid_t pid;
	
	char buf[100] = {'\0'};
	char tmp[100] = {'\0'};
	pipe(fd);
	pipe(fd2);
	
	if ((pid = fork()) == -1)
	{
		printf("Fork failed\n");
		exit(-1);
	}
	
	fprintf(stderr, "Execute cmd\n");
	if (pid == 0)
	{
		//child process
		close(fd[0]);
		close(fd2[1]);
		dup2(fd[1], 1);
		dup2(fd2[0], 0);
		fprintf(stderr, "Child:\n");
		//execlp("passwd", NULL);
		execlp("/usr/bin/passwd", "passwd", "root", NULL);
		fprintf(stderr, "Child2:\n");
	}
	else if (pid > 0)
	{
		//parent process
		close(fd[1]);
		close(fd2[0]);
		dup2(fd[0], 0);
		dup2(fd2[1], 1);
		
		fprintf(stderr, "Parent:\n");
		read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
		sleep(1);
		read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
		sleep(1);
		read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
		sleep(1);
		read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
	}
	
	return 0;
}

int main(void)
{
	FILE *fp = NULL;
	char buffer[80];
	char passwd[20] = {"te st\n"};
	int len = 0;
	
	fp = popen("passwd test", "w");
	if (fp != NULL)
	{
		//sleep(1);
		//fgets(buffer, sizeof(buffer), fp);
		//printf("Buffer1=%s\n", buffer);
		//sleep(1);
		len = fwrite(passwd, strlen(passwd), 1, fp);
		//printf("len1=%d\n", len);
		//sleep(1);
		//fgets(buffer, sizeof(buffer), fp);
		//printf("Buffer2=%s\n", buffer);
		//sleep(1);
		len = fwrite(passwd, strlen(passwd), 1, fp);
		//printf("len2=%d\n", len);
		//sleep(1);
		//fgets(buffer, sizeof(buffer), fp);
		//printf("Buffer3=%s\n", buffer);
	}

	if (NULL != fp)
		pclose(fp);
}
#if 0
 #define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    int masterfd,slavefd;
    char *slavedevice;
    char buf[200];
    if((masterfd = posix_openpt(O_RDWR|O_NOCTTY)) == -1)
    {
        perror("posix_openpt() error");
        exit(-1);
    }
    if(grantpt(masterfd) == -1)
    {
        perror("grantpt() error");
        exit(-1);
    }
    if(unlockpt(masterfd) == -1)
    {
        perror("unlockpt() error");
        exit(-1);
    }
    //if((slavedevice=ptsname(masterfd)) == NULL)
    if ((ptsname_r(masterfd, buf, 200)) < 0)
    {
        perror("ptsname() error");
        exit(-1);
    }
    printf("slave device is: %s\n", buf);
    exit(0);
}
//#else

# include <stdio.h>  
# include <stdlib.h>  
# include <string.h>  
# include <unistd.h>  
# include <sys/types.h>  
# include <linux/limits.h>  
# include <pty.h> /* for openpty and forkpty */  
#include <utmp.h> /* for login_tty */  
#include <time.h>  
# include <pty.h> /* for openpty and forkpty */  
#include <utmp.h> /* for login_tty */  
int main(int argc, char *argv[])  
{  
	int rtnVal=0;  
	int mpty, spty, c=0, dev=0;  
	//char *pName=NULL;  
	char pName[20];
	char ptyBuff[100] = {'\0'};  
	char sptyname[20] = {'\0'};  
	rtnVal = openpty(&mpty, &spty, NULL, NULL, NULL);
	/**该函数遍历系统中的伪终端匹配对，如果能找到一组未使用的，则返回1，否则返回－1，
		成功返回时mpty会带出匹配对中主设备的文件描述符，spty会带出从设备的文件描述符，
		* 第三个实参如果不空的话，它会带出从设备的路径名！后边两个参数是在设置终端属性，
		* 一般是不要的，据说伪终端对属性设置是忽略的*/
	// Check if Pseudo-Term pair was created  
	if(rtnVal != -1)
	{  
		//pName = ptsname(mpty);//get slave device name, the arg is the master device  
		ptsname_r(mpty, pName, 20);
		printf("Name of slave side is <%s> fd = %d\n", pName, spty);
 
		strcpy(sptyname, pName);  
		printf("my sptyname is %s\n",sptyname);  
		//test write to mpty and read from spty*************  
		char temp[50] = {"hello\nworld ! i have write to mpty!"};  
		char temp2[100] = {'\0'};  
		c = write(mpty,temp,5);  
		if(c <=0)  
			printf("ERROR : can not write to mpty\n");  
		sleep(3);  
		printf("write %d charactors to mpty success\n",c);  
		sleep(3);  
		printf("try to read from spty\n");  
		sleep(3); 
		printf("start read from spty\n");
		c = read(spty,temp2,5);  
		if(c <=0) 
			printf("ERROR : can not read from mpty\n");  
		printf("read from spty  %d charactors success\n",c);
		printf("\n>>>>>  %s  <<<<<\n\n___________________\n",temp2);
		//**************************************************  

		// Go into loop and read what is sent to Slave side of pair  
		while(1)  
		{  
			c = read(mpty, ptyBuff, 100);  
			if(c > 0)  
			{  
				printf("###-<%d>\n", c);  
				printf("buff:__|%s",ptyBuff);  
			}
		}  
	}
	else  
	{  
		printf("PseudoTerm, creation failed...\n");  
	}
	return rtnVal;  
}
/* Once Solaris has openpty(), this is going to be removed. */  
/*int openpty(int *amaster, int *aslave, char *name,  
            struct termios *termp, struct winsize *winp)  
{  
        const char *slave;  
        int mfd = -1, sfd = -1;  
        *amaster = *aslave = -1;  
        mfd = open("/dev/ptmx", O_RDWR | O_NOCTTY);  
        if (mfd < 0)  
                goto err;  
        if (grantpt(mfd) == -1 || unlockpt(mfd) == -1)  
                goto err;  
        if ((slave = ptsname(mfd)) == NULL)  
                goto err;  
        if ((sfd = open(slave, O_RDONLY | O_NOCTTY)) == -1)  
                goto err;  
        if (ioctl(sfd, I_PUSH, "ptem") == -1 ||  
            (termp != NULL && tcgetattr(sfd, termp) < 0))  
                goto err;  
        if (amaster)  
                *amaster = mfd;  
        if (aslave)  
                *aslave = sfd;  
        if (winp)  
                ioctl(sfd, TIOCSWINSZ, winp);  
        return 0;  
err:  
        if (sfd != -1)  
                close(sfd);  
        close(mfd);  
        return -1;  
}
*/
//int main(void)
//{
	//execute_cmd();
//}
#endif
