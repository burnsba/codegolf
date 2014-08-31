#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

uint64_t usec_diff(struct timeval start_time)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return
        (uint64_t)((tv.tv_usec + tv.tv_sec*1e6) - 
            (start_time.tv_usec + start_time.tv_sec*1e6));
}

void bubble_sort(int *arr, int arr_len)
{
	int i,j, swap;
	
	for (i = 0 ; i < ( arr_len - 1 ); i++)
	{
		for (j = 0 ; j < arr_len - i - 1; j++)
		{
			if (arr[j] > arr[j+1])
			/* For decreasing order use < */
			{
				swap = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = swap;
			}
		}
	}
}

int main()
{
	struct timeval start_time;
	int array_length = 10000;
	int *arr;
	
	int i,j;
	uint64_t msec_time = 0;
	
	arr = (int *)malloc(sizeof(int) * array_length);
	memset(arr, 0, array_length);

	for(i=0,j=array_length; i<array_length && j>0; i++, j--)
	{
		arr[i] = j;
	}

	gettimeofday(&start_time,NULL);

	bubble_sort(arr, array_length);

	msec_time = usec_diff(start_time) / 1000;
	
	printf("Worst case time in ms is %d\n", msec_time);

	free(arr);

	arr = (int *)malloc(sizeof(int) * array_length);
	memset(arr, 0, array_length);

	gettimeofday(&start_time,NULL);

	bubble_sort(arr, array_length);

	msec_time = usec_diff(start_time) / 1000;

	printf("Best case time in ms is %d\n", msec_time);

	return 0;
}