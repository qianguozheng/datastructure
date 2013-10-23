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
// heap sort algorithm
static int adjust_heap(int *arr, int size)
{
   int lchild, rchild;
   int i = 1, temp;
   while (2*i <= size && (2*i+1) <= size)
   {
       lchild = 2*i - 1;
       rchild = 2*i;
       if (arr[lchild] > arr[i-1])
       {
            temp = arr[lchild];
            arr[lchild] = arr[i-1];
            arr[i-1] = temp;
       }
       else if (arr[rchild] > arr[i-1])
       {
            temp = arr[rchild];
            arr[rchild] = arr[i-1];
            arr[i-1] = temp;
       }
       i++;
   }
}
int heapsort(int *arr, int size)
{
	int temp;
    int i = 1;
    while ((2*i <= size) && (2*i - 1 <= size))
    {
        adjust_heap(arr, 2*i);
        i++;
    }
}
