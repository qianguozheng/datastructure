#include <stdio.h>
#include <stdlib.h>
#include "sort.h"

// bubble sort algorithm
int bubble(int *arr, int size)
{
	int temp;
	int i, j;
	for (i = 0; i< size; i++)
	{
		for (j = size - 1; j > i; j--)
		{
			if (arr[j-1] > arr[j])
			{
				temp = arr[j -1];
				arr[j-1] = arr[j];
				arr[j] = temp;
			}
		}
	}
	return 0;
}
