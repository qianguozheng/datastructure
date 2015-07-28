#include<stdio.h>

int main(int argc, char *argv[])
{
	int i = 0, j = 0, k = 0;
	int arr[10] = {9,3,4,5,3,3,3,4,5,3};
	for (i = 0; i< 10-k; i++)
	{
		if (arr[i]==3)
		{
			
			for(j = i; (j+1)< 10-k; j++)
			{
				arr[j] = arr[j+1];
			}
			k ++;
			i--;
		}
		
		printf("arr[%d]=%d\t", i, arr[i]);
	}
	
	for (i = 0; i< 10-k; i++)
	{
		
		printf("\n###arr[%d]=%d", i, arr[i]);
	}
	for (i = 0; i< 10; i++)
	{
		
		printf("\n###arr[%d]=%d", i, arr[i]);
	}
}
