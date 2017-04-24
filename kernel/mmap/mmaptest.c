#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

int main()
{
	int fd;
	char *start;
	char *buf;
	
	fd = open("/dev/fuck", O_RDWR);
	buf = (char *) malloc(100);
	memset(buf, 0, 100);
	start = mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	
	
	//Read data
	strcpy(buf, start);
	sleep(1);
	printf("buf 1=%s\n", buf);
	
	//Write data
	strcpy(start, "Buf is not NUll@");
	memset(buf, 0, 100);
	strcpy(buf, start);
	sleep(1);
	printf("buf 2=%s\n", buf);
	
	munmap(start, 100);
	free(buf);
	close(fd);
	return 0;
}
