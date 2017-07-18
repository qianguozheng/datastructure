#include <stdio.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#define IOC_MAGIC 'k'
#define IOCTL_HELLO _IO(IOC_MAGIC,0)

int main(){
	int fd;
	fd = open("/dev/temp", O_WRONLY);
	if (-1 == fd){
		printf("Error open\n");
		exit(-1);
	}
	ioctl(fd, IOCTL_HELLO); //ioctl call
	close(fd);
}
