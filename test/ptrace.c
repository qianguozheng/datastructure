#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/reg.h>
//#include <linux/user.h>
int main()
{
	pid_t child;
	long orig_eax;
	child = fork();

	if (0 == child){
		ptrace(PTRACE_TRACEME, 0, NULL, NULL);
		execl("/bin/ls", "ls", NULL);
	}
	else{
		wait(NULL);
		orig_eax = ptrace(PTRACE_PEEKUSER, 
				child, 4 * ORIG_EAX, NULL);
		printf("The child made a system call %ld", orig_eax);

		ptrace(PTRACE_CONT, child, NULL, NULL);
	}
	return 0;
}
