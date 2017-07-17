#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/resource.h>
static int setcoredump(void)
{
    struct rlimit limit;
    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_CORE, &limit) != 0) {
        printf("setrlimit() failed with %s\n", strerror(errno));
        return -1;
    }

    if (getrlimit(RLIMIT_CORE, &limit) != 0) {
        printf("call getrlimit error, %s", strerror(errno));
        return -1;
    }

    printf("The soft limit for CORE after is %lu\n", limit.rlim_cur);
    printf("The hard limit for CORE after is %lu  %lu\n", limit.rlim_max, RLIM_INFINITY);
    
#if 1
	if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
        printf("call getrlimit error, %s", strerror(errno));
        return -1;
    }
    printf("The soft limit for NOFILE after is %lu\n", limit.rlim_cur);
    printf("The hard limit for NOFILE after is %lu  %lu\n", limit.rlim_max, RLIM_INFINITY);

    
    limit.rlim_cur = limit.rlim_max;
    limit.rlim_max = limit.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
        printf("setrlimit() failed with %s\n", strerror(errno));
        return -1;
    }
    
    printf("The soft limit for RLIMIT_NOFILE after is %lu\n", limit.rlim_cur);
    printf("The hard limit for RLIMIT_NOFILE after is %lu  %lu\n", limit.rlim_max, RLIM_INFINITY);
#endif
    // 测试生成coredump
    //     //char *p = NULL;
    //         //*p = 123;
                 return 0;
}
unsigned int generate_rand ()
{
	int fd;
	unsigned char  n = 0;
	fd = open ("/dev/urandom", O_RDONLY);
	if (fd > 0){
		read (fd, &n, sizeof (n));
		printf("n=%llu\n", n);
	}
	close (fd);
	return (n%30+1);
}

int main(){
	setcoredump();
	printf("random=%u\n", generate_rand());
}
