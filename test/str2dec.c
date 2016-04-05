/******************
 * Email: guozhengqian0825@126.com
 * Date: 2014-9-16
 ******************/
#include <stdio.h>
#include <stdlib.h>

typedef struct table
{
	char name;
	int value;
} table;

table hex2dec[16] =
{
	'0', 0,
	'1', 1,
	'2', 2,
	'3', 3,
	'4', 4,
	'5', 5,
	'6', 6,
	'7', 7,
	'8', 8,
	'9', 9,
	'A', 10,
	'B', 11,
	'C', 12,
	'D', 13,
	'E', 14,
	'F', 15
};
// translate string to decimal
int str2dec(char *str)
{
	char *p = str;
	int a = 0, b = 0, i = 0;
	if (!p)
	{
		return -1;
	}
	
	for (i = 0; i< 16; i++)
	{
		if (hex2dec[i].name == toupper(p[0]))
		{
			a = hex2dec[i].value;
		}
		if (hex2dec[i].name == toupper(p[1]))
		{
			b = hex2dec[i].value;
		}
	}
	return (a*16+b);
}

#if 0
int main(int argc, char *argv[])
{
    printf("AB=%d\n", str2dec("AB"));
    return 0;
}
#endif
