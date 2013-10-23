#include <stdio.h>
#include <stdlib.h>
#include "util.h"

void print(int *arr, int size)
{
	int i = 0;
	for (i = 0; i<size; i++)
	{
		printf("%d ", arr[i]);
	}
	printf("\n");
}
// swap a and b if a > b
int swap(int *a, int *b)
{
	int temp = 0;

	if (*a > *b)
	{
		temp = *a;
		*a = *b;
		*b = temp; 
	}
	return 0;
}
