#include <stdio.h>

int main()
{
	int a = 0;
	int b = 3;
	printf("a=%d\n", (b & (a << 1)));
	printf("a=%d\n", (b & ~(1 << 1)));
}
