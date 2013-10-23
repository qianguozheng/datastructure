/*************************************
 * Author: Qianguozheng
 * E-mail: guozhengqian0825@126.com
 * Date: 2013-10-23
 *************************************/
#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

int main(int argc, char *argv[])
{
	int arr[10] = {0, 10, 9, 7, 2, 6, 4, 5, 3, 8};
	printf ("before sort arr:\n");
	print(arr, 10);
	//bubble(arr, 10);
    heapsort(arr, 10);
	printf ("sorted arr:\n");
	print(arr, 10);
	return 0;
}
