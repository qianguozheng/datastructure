#include <stdio.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
//http://www.cnblogs.com/pengdonglin137/p/3310032.html

void test()
{
	glob_t buf;
	int i;
	
	glob("/dev/*", GLOB_NOSORT, NULL, &buf);
	
	for(i = 0; i < buf.gl_pathc; i++)
	{
		printf("buf.gl_pathv[%d]=%s\n", i, (buf.gl_pathv[i]));
	}
	globfree(&buf);
}

static void print_gl(glob_t *gl)
{
	for(unsigned int i=0; i < gl->gl_pathc; ++i)
		printf("%s\n", gl->gl_pathv[i]);
	printf("+++++++++++++++++++++++++++++++++++\n");
}
//类似于模式匹配  ./a.out /tmp/*

static int test_fun(int argc, char *argv[])
{
	glob_t gl;
	for (int i=1; i<argc; ++i){
		gl.gl_offs = 0;
		glob(argv[i], GLOB_TILDE, 0, &gl);
		print_gl(&gl);
		globfree(&gl);
	}
	
	return 0;
}

int main(int argc, const char *argv[])
{
	if (argc > 1)
		test_fun(argc, argv);
	else
		printf("./mytest path_list \n");
	return 0;
}
