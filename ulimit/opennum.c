#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>

/**
 * http://blog.sina.com.cn/s/blog_6a1837e901010ar4.html 
 * 
 */
int main(void)
{
     struct rlimit r;
     if(getrlimit(RLIMIT_NOFILE,&r)<0)
     {
         fprintf(stderr,"getrlimit error\n");
         exit(1);
     }
     printf("RLIMIT_NOFILE cur:%d\n",r.rlim_cur);
     printf("RLIMIT_NOFILE max:%d\n",r.rlim_max);
    
     //r.rlim_cur=100;
     //r.rlim_max=200;
     r.rlim_cur = 4096;
     r.rlim_max = 4096;
     
     if (setrlimit(RLIMIT_NOFILE,&r)<0)
     {
         fprintf(stderr,"setrlimit error\n");
         exit(1);
     }
    
     if(getrlimit(RLIMIT_NOFILE,&r)<0)
     {
         fprintf(stderr,"getrlimit error\n");
         exit(1);
     }
     printf("RLIMIT_NOFILE cur:%d\n",r.rlim_cur);
     printf("RLIMIT_NOFILE max:%d\n",r.rlim_max);
     return 0;
}
