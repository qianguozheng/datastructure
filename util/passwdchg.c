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
		//execlp("/usr/bin/passwd", "passwd", "root", NULL);
		system("passwd root");
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
		//read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
		sleep(1);
		//read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
		sleep(1);
		//read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
		sleep(1);
		//read(1, buf, sizeof(buf));
		fprintf(stderr, "buf=%s\n", buf);
	}
	
	return 0;
}

int main(void)
{
	execute_cmd();
}
