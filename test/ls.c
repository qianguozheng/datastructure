#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


void show_attr(char *name)
{
	struct stat buf;
	struct passwd *pwd;
	struct group *grp;
	char type;
	char permission[9];
	int i = 0;
	
	memset(permission, '-', 9*sizeof(char));
	printf("name=[%s]\n", name);
	if (!stat(name, &buf))
	{
		printf("Got Here\n");
		//get the type of file
		if (S_ISLNK(buf.st_mode))
			type = 'l';
		else if (S_ISREG(buf.st_mode))
			type = '-';
		else if (S_ISDIR(buf.st_mode))
			type = 'd';
		else if (S_ISCHR(buf.st_mode))
			type = 'c';
		else if (S_ISBLK(buf.st_mode))
			type = 'b';
		else if (S_ISFIFO(buf.st_mode))
			type = 'p';
		else if (S_ISSOCK(buf.st_mode))
			type = 's';
		else
			type = '*';
			
		//get the permission of file
		if (buf.st_mode & S_IRUSR)
		{
			permission[0] = 'r';
		}
		if (buf.st_mode & S_IWUSR)
		{
			permission[1] = 'w';
		}
		if (buf.st_mode & S_IXUSR)
		{
			permission[2] = 'x';
		}
		if (buf.st_mode & S_IRGRP)
		{
			permission[3] = 'r';
		}
		if (buf.st_mode & S_IWGRP)
		{
			permission[4] = 'w';
		}
		if (buf.st_mode & S_IXGRP)
		{
			permission[5] = 'x';
		}
		if (buf.st_mode & S_IROTH)
		{
			permission[6] = 'r';
		}
		if (buf.st_mode & S_IWOTH)
		{
			permission[7] = 'w';
		}
		if (buf.st_mode & S_IXOTH)
		{
			permission[8] = 'x';
		}
		printf("Got Here2\n");
		// get the user name and group name
		
		pwd = getpwuid(buf.st_uid);
		printf("Got Here3\n");
		grp = getgrgid(buf.st_gid);
		printf("Got Here4\n");
		if (NULL == pwd)
		{
			printf("pw is null \n");
			exit(1);
		}
		if (NULL == grp)
		{
			printf("grp is null \n");
			exit(1);
		}
		printf("Got Here5\n");
		printf("%c", type);
		printf("Got Here6\n");
		
		i = 0;
		while (i<9)
		{
			printf("%c", permission[i]);
			i++;
		}
		printf("Got Here7\n");
		
		printf("%2d ", buf.st_nlink);
		printf("Got Here8\n");
		printf("%-4s", pwd->pw_name);
		printf("Got Here9\n");
		printf("%-4s", grp->gr_name);
		printf("Got Here10\n");
		
		printf("%6ld ", buf.st_size);
		printf("Got Here11\n");
		printf("%ld\n", buf.st_mtime);
		//printf("%s", ctime(&(buf.st_mtime)));
		/**
		 * [51206.712157] ls[31218]: segfault at 2ae5bc80 ip 00007fc42ab208d4 sp 00007ffd2151c128 error 4 in libc-2.21.so[7fc42aa95000+1c0000]
		 * 
		 * 
		 * Breakpoint 2, show_attr (name=0x7fffffffe14f "ls.c") at ls.c:117
			117			printf("%s", ctime(&(buf.st_mtime)));
			(gdb) n

			Program received signal SIGSEGV, Segmentation fault.
			0x00007ffff7a5d4b2 in _IO_vfprintf_internal (s=<optimized out>, format=<optimized out>, ap=ap@entry=0x7fffffffdaa8) at vfprintf.c:1642
			1642	vfprintf.c: No such file or directory.

		 * */
		//printf("%.12s",ctime(&buf.st_mtime)+4);
		printf("Got Here12\n");
		printf(" %s\n", name);
		printf("Got Here13\n");
	}
	else
	{
		printf("can't get the state of %s \n", name);
		exit(1);
	}
}
int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Usage: ./ls xxx\n");
		return;
	}
	show_attr(argv[1]);
}
