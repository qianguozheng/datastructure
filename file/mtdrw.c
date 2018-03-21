#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <mtd/mtd-user.h>

int mtdsize=0;
int erasesize=0;

int mtd_open(const char *mtd)
{
	FILE *fp;
	char dev[PATH_MAX];
	int i;
	int ret;
	int flags = O_RDWR | O_SYNC;
	
	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) {
				snprintf(dev, sizeof(dev), "/dev/mtd%s/%d", (0 ? "block" : ""), i);
				if ((ret=open(dev, flags))<0) {
					snprintf(dev, sizeof(dev), "/dev/mtd%s%d", (0 ? "block" : ""), i);
					printf("dev=[%s]\n", dev);
					ret=open(dev, flags);
				}
				fclose(fp);
				
				return ret;
			}
		}
		fclose(fp);
	}

	return open(mtd, flags);
}

int mtd_check_open(const char *mtd)
{
	struct mtd_info_user mtdInfo;
	int fd;

	fd = mtd_open(mtd);
	if(fd < 0) {
		perror("Could not open mtd device\n");
		return -1;
	}

	if(ioctl(fd, MEMGETINFO, &mtdInfo)) {
		perror("Could not get MTD device info from \n");
		close(fd);
		return -1;
	}
	mtdsize = mtdInfo.size;
	erasesize = mtdInfo.erasesize;

	return fd;
}



static int
mtd_unlock()
{
	struct erase_info_user mtdLockInfo;
	char *next = NULL;
	char *str = NULL;
	int fd;

	
	//fd = mtd_check_open("/dev/mtdblock0");
	fd = mtd_check_open("u-boot-env");
	if(fd < 0) {
		fprintf(stderr, "Could not open mtd device:\n");
		exit(1);
	}

	fprintf(stderr, "Unlocking %d...\n", mtdsize);

	mtdLockInfo.start = 0;
	mtdLockInfo.length = mtdsize;
	ioctl(fd, MEMUNLOCK, &mtdLockInfo);
	close(fd);

	return 0;
}



int main(void) {
#if 0
	FILE *fp = fopen("/dev/mtdblock0", "r+");
	char buf[12];
	char assigned[] = "aabbccddee00";
	char reset[] = "ffffffffffff";
	memset(buf, 0, sizeof(buf));
	int i=0;
	
	if (fp) {
		fseek(fp, 0x500, SEEK_SET);
		fread(buf, 12, 1, fp);
		printf("Origin Data\n");
		for (i=0; i< 12;i++)
		 printf("buf[%d] = %x\t",i, buf[i]);
		 
		printf("\nbuf=%s\n", buf);
		printf("\n");
		memset(buf, 0, sizeof(buf));
		
		fseek(fp, 0x500, SEEK_SET);
		fwrite(assigned, strlen(assigned), 1, fp);
		fflush(fp);
		
		printf("Setting Data\n");
		fseek(fp, 0x500, SEEK_SET);
		fread(buf, 12, 1, fp);
		for (i=0; i< 12;i++)
		 printf("buf[%d] = %x\t",i, buf[i]);
		
		printf("\nbuf=%s\n", buf);
		memset(buf, 0, sizeof(buf));
		
		//fseek(fp, 0x1b8a0, SEEK_SET);
		//fwrite(reset, strlen(reset), 1, fp);
		//fflush(fp);
		
		//printf("\nReset Data\n");
		//fseek(fp, 0x1b8a0, SEEK_SET);
		//fread(buf, 12, 1, fp);
		//for (i=0; i< 12;i++)
		// printf("buf[%d] = %x\t",i, buf[i]);
		
		
		fclose(fp);
	} else {
		perror("open file failed\n");
	}
#endif
	int fd = 0;
	
	char buf[12];
	char assigned[] = "aabbccddee00";
	//char reset[] = "ffffffffffff";
	int reset = 0;
	//char reset[] = "000000000000";
	memset(buf, 0, sizeof(buf));
	int i=0, ret = 0;
	
	//mtd_unlock();
	
	printf("xxxx\n");
	fd = open("/dev/mtdblock1", O_RDWR|O_SYNC);
	
	if (fd > 0) {
		lseek(fd, 0x500, SEEK_SET);
		ret = read(fd, buf, 12);
		for (i=0; i< 12;i++)
		 printf("buf[%d] = %x\t",i, buf[i]);
		printf("ret=%d\n", ret);
		
		lseek(fd, 0x500, SEEK_SET);
		printf("Setting Data\n");
		ret = write(fd, assigned, strlen(assigned));
		printf("ret=%d\n", ret);
		
		lseek(fd, 0x500, SEEK_SET);
		ret = read(fd, buf, 12);
		for (i=0; i< 12;i++)
		 printf("buf[%d] = %x\t",i, buf[i]);
		printf("ret=%d\n", ret);
		
		printf("Reset to Origing\n");
		lseek(fd, 0x500, SEEK_SET);
		ret = write(fd, &reset, 4);
		ret += write(fd, &reset, 4);
		ret += write(fd, &reset, 4);
		
		lseek(fd, 0x500, SEEK_SET);
		ret = read(fd, buf, 12);
		for (i=0; i< 12;i++)
		 printf("buf[%d] = %x\t",i, buf[i]);
		printf("ret=%d\n", ret);
		
		//sync(fd);
		close(fd);
	} else {
		perror("Open Failed\n");
	}
	
	return 0;
}
