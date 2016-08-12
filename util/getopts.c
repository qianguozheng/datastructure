#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "a:b:")) != -1)
	{
		switch (opt)
		{
			case 'a': printf("a\t%x\n", optarg);
				break;
			case 'b': printf("a\t%x\n", optarg);
				break;
		}
	}
	
	printf("%x\t%x\n", argv[2], argv[4]);
}
