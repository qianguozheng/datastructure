#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define _INTSIZEOF(n) ((sizeof(n)+sizeof(int)-1)&~(sizeof(int)-1))


int max(int n, ...)
{
	va_list ap;
	va_start(ap, n);

	int maximum = -1;
	int temp, i=0;
	for(i=0; i<n; i++)
	{
		temp = va_arg(ap, int);
		if (maximum < temp) maximum = temp;
	}
	va_end(ap);

	return maximum;
}

int main(int argc, char *argv[])
{
	int a=1;
	/*char b='a';
	float c=1.0;
	double d=1.0;
	long f=1L;

	printf("int a=%lu\nchar b=%lu\nfloat c=%lu\ndouble d=%lu\nlong f=%lu\n"
		, _INTSIZEOF(a)
		, _INTSIZEOF(b)
		, _INTSIZEOF(c)
		, _INTSIZEOF(d)
		, _INISIZEOF(f));
	*/
	char b='a';
	printf("int a=%lu\nchar b=%lu\n", _INTSIZEOF(a), _INTSIZEOF(b));

	printf("maxmum=%d\n", max(4,5,6,2,1));
	printf("maxmum=%d\n", max(8,51,61,22,11,22,3,4,5));
}
