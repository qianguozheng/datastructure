#include <unistd.h>  
#include <stdio.h>  
#include <sys/file.h>  
  
main()  
{  
    const char filename[]  = "mylockfile";  
    char a[1];  
    int fd = open (filename,O_CREAT|O_WRONLY , 0644);  
    int lock =  lockf(fd,F_TLOCK,0);  
    if (fd == -1)  
    {  
        perror("open file");  
       return -1;  
    }  
    if (lock == -1)  
    {  
        perror("lock fail");  
        return -1;  
    }  
     //use unlink then not have any file ,then second process can do same thing again  
    //unlink(filename);  
    while(1)  
    {  
        sleep(10);  
        //test core dump  
        printf("%s",a[2]);  
    }  
}  
