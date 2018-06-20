#include <fcntl.h>
#include <utime.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>

///Hack tool. clear file content and don't change the time of file

struct utimbuf1 {
	time_t actime;
	time_t modtime;
};

int main(int argc, char *argv[]) {
	int i, fd;
	struct stat statbuf;
	struct utimbuf1 timebuf;
	
	for (i=1; i< argc; i++) {
		if (stat(argv[i], &statbuf) < 0) {
			printf("%s: stat error,", argv[i]);
			continue;
		}
		
		if ((fd = open(argv[i], O_RDWR|O_TRUNC)) < 0) {
			printf("%s: open error", argv[i]);
			continue;
		}
		
		close(fd);
		
		timebuf.actime = statbuf.st_atime;
		timebuf.modtime = statbuf.st_mtime;
		
		if (utime(argv[i], &timebuf) < 0) {
			printf("%s: utime error", argv[i]);
			continue;
		}
	}
	exit(0);
}
