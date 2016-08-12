/*************************************************************************
	> File Name: testcap.c
	> Author: 
	> Mail: 
	> Created Time: 2016年06月24日 星期五 10时39分46秒
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/capability.h>

extern int errno;
void whoami(void)
{
    printf("uid=%i euid=%i gid=%i\n", getuid(), geteuid(), getgid());
}

void listCaps()
{
    cap_t caps = cap_get_proc();
    ssize_t y = 0;
    printf("The process %d was give capabilities %s\n", 
        (int)getpid(), cap_to_text(caps, &y));
    fflush(0);
    cap_free(caps);
}

int main(int argc, char **argv)
{
    int stat;
    whoami();

    stat = setuid(geteuid());
    pid_t parentPid = getpid();

    if (!parentPid)
        return 1;

    cap_t caps = cap_init();
    cap_value_t capList[5] = {CAP_NET_RAW, CAP_NET_BIND_SERVICE, CAP_SETUID, CAP_SETGID, CAP_SETPCAP};
    unsigned num_caps = 5;
    cap_set_flag(caps, CAP_EFFECTIVE, num_caps, capList, CAP_SET);
    cap_set_flag(caps, CAP_INHERITABLE, num_caps, capList, CAP_SET);
    cap_set_flag(caps, CAP_PERMITTED, num_caps, capList, CAP_SET);

    if (cap_set_proc(caps)){
        perror("capset()");

        exit(0);
    }

    listCaps();

    cap_free(caps);

    struct __user_cap_header_struct cap_header_data;
    cap_user_header_t cap_header = &cap_header_data;

    struct __user_cap_data_struct cap_data_data;
    cap_user_data_t cap_data = &cap_data_data;

    cap_header->pid = getpid();
    cap_header->version = _LINUX_CAPABILITY_VERSION_1;

    if (capget(cap_header, cap_data) < 0)
    {
        perror("capget");
        exit(1);
    }

    printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective, cap_data->permitted, cap_data->inheritable);

    sleep(60);
    return 0;
}
