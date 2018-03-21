#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define STACK_SIZE (1 * 1024 * 1024 *1024)

char child_stack[STACK_SIZE];

int child_main(void* args) {
    pid_t child_pid = getpid();
    printf("I'm child process and my pid is %d \n", child_pid);
    // 子进程会被放到clone系统调用新创建的pid命名空间中, 所以其pid应该为1
    sleep(300);
    // 命名空间中的所有进程退出后该命名空间的inode将会被删除, 为后续操作保留它
    return 0;
}

int main() {
    /* Clone */
    pid_t child_pid = clone(child_main, child_stack + STACK_SIZE, \
        CLONE_NEWPID | SIGCHLD, NULL);
    if(child_pid < 0) {
        perror("clone failed");
    }

    /* Unshare */
    int ret = unshare(CLONE_NEWPID); // 父进程调用unshare, 创建了一个新的命名空间,
    //但不会创建子进程. 之后再创建的子进程将会被加入到新的命名空间中
    if (ret < 0) {
        perror("unshare failed");
    }
    int fpid = fork();
    if (fpid < 0) {
        perror("fork error");
    } else if (fpid == 0) {
        printf("I am child process. My pid is %d  \n", getpid());
        // Fork后的子进程会被加入到unshare创建的命名空间中, 所以pid应该为1
        exit(0);
    } else {
    }
    waitpid(fpid, NULL, 0);

    /* Setns */
    char path[80] = "";
    sprintf(path, "/proc/%d/ns/pid", child_pid);
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        perror("open error");
    if (setns(fd, 0) == -1)
    // setns并不会改变当前进程的命名空间, 而是会设置之后创建的子进程的命名空间
        perror("setns error");
    close(fd);

    int npid = fork();
    if (npid < 0) {
        perror("fork error");
    } else if (npid == 0) {
        printf("I am child process. My pid is %d  \n", getpid());
        // 新的子进程会被加入到第一个子进程的pid命名空间中, 所以其pid应该为2
        exit(0);
    } else {
    }
    return 0;
}
