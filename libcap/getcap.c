/*************************************************************************
	> File Name: getcap.c
	> Author: 
	> Mail: 
	> Created Time: 2016年06月24日 星期五 10时32分08秒
 ************************************************************************/

#include<stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/capability.h>

#include <errno.h>

int main()
{
    struct __user_cap_header_struct cap_header_data;
    cap_user_header_t cap_header = &cap_header_data;

    struct __user_cap_data_struct cap_data_data;
    cap_user_data_t cap_data = &cap_data_data;

    cap_header->pid = getpid();
    cap_header->version = _LINUX_CAPABILITY_VERSION_1;

    if (capget(cap_header, cap_data) < 0)
    {
        perror("cap_header fuck capget failed");
        exit(1);
    }

    printf("Cap data 0x%x, 0x%x, 0x%x\n", cap_data->effective, cap_data->permitted, cap_data->inheritable);
}
