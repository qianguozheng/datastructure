#include <stdio.h>

int main()
{
	int a = 30 * 24 * 3600;
	int b = 0x7FFFFFFF;
	printf("int=%d\ncache-time=%d\ntotallen=%d\n", sizeof(int), a, b);
	return 0;
}
