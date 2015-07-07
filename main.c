/*************************************
 * Author: Qianguozheng
 * E-mail: guozhengqian0825@126.com
 * Date: 2013-10-23
 *************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "base64.h"
int main(int argc, char *argv[])
{
#if 0
	int arr[10] = {5, 10, 9, 7, 2, 6, 4, 5, 3, 8};
	printf ("before sort arr:\n");
	print(arr, 10);
	//bubble(arr, 10);
    heapsort(arr, 10);
	printf ("sorted arr:\n");
	print(arr, 10);
#endif
	char str[128], encode[128];//= "Hello world";
	snprintf(str, sizeof(str)-1, "%s", "Hello World");
	snprintf(encode, sizeof(encode)-1, "%s", base64_encode(str, strlen(str)));
	printf("base64_encode:%s\n", encode);

	printf("base64_decode:%s\n", base64_decode(encode, strlen(encode)));

	return 0;
}
