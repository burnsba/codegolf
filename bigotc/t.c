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

int main(int argc, char** argv)
{

	int array_length = 10000;
	int *arr;
	
	int i,j;
	
	int input = atoi(argv[1]);
	int target = input * 1000;
	
	printf("target is: %d\n", target);
	fflush(stdout);
	
	uint64_t msec_loop_time = 0;
	int inv_adjust = 256;
	
	while(1)
	{
		
    
		arr = (int *)malloc(sizeof(int) * array_length);
		memset(arr, 0, array_length);
	
		for(i=0,j=array_length; i<array_length && j>0; i++, j--)
		{
			arr[i] = j;
		}
		
		struct timeval start_time;
		gettimeofday(&start_time,NULL);
	
		bubble_sort(arr, array_length);
		
		msec_loop_time = usec_diff(start_time) / 1000;
		
		if (msec_loop_time > target * 0.95 && msec_loop_time < target * 1.05)
		{
			free(arr);
			
			printf("just right: %d\n", array_length);
			fflush(stdout);
			
			break;
		}
		else if (msec_loop_time > target)
		{
			free(arr);
			
			inv_adjust = inv_adjust >> 1;
			array_length = array_length - inv_adjust;
			
			printf("too large: %d\n", array_length);
			fflush(stdout);
		}
		else // less than target
		{
			free(arr);
			
			inv_adjust = inv_adjust << 1;
			array_length = array_length + inv_adjust;
			
			printf("too small: %d\n", array_length);
			fflush(stdout);
		}

		printf("loop time: %d\n", msec_loop_time);
		fflush(stdout);
	}
	
	printf("final loop time: %d\n", msec_loop_time);
	
	arr = (int *)malloc(sizeof(int) * array_length);
	memset(arr, 0, array_length);
	
	struct timeval t;
	gettimeofday(&t,NULL);

	bubble_sort(arr, array_length);
	
	msec_loop_time = usec_diff(t) / 1000;
	
	printf("Square root is approx: %f\n", (double)(msec_loop_time / 1000.0));
	
	return 0;
}