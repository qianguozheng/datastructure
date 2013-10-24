#include <stdio.h>
#include <stdlib.h>
#include "sort.h"
#include "util.h"

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
// refer to: sina blog Better Linux, better me
static int adjust_heap(int *arr, int i, int size)
{
	int lchild = 2*i;
	int rchild = 2*i + 1;
	int max = i;
	
	if (i <= size/2)
	{
		if (lchild < size && arr[lchild] > arr[max])
		{
			max = lchild;
		}
		if (rchild < size && arr[rchild] > arr[max])
		{
			max = rchild;
		}
		if (max != i)
		{
			swap(&arr[i], &arr[max]);
			adjust_heap(arr, max, size);
		}
	}
}
static int build_heap(int *arr, int size)
{
	int k = 0;
	for (k = size/2; k >=0; k--)
	{
		adjust_heap(arr, k, size);
	}
	return 0;
}
int heapsort(int *arr, int size)
{
	int i;
	build_heap(arr, size);
	for (i = size-1; i>=0; i--)
	{
		swap(&arr[0], &arr[i]);
		adjust_heap(arr, 0, i);
	}
}
