#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define PI               3.141592653589793238462643383279502884197169399375105
#define PI_TIMES_2       6.283185307179586476925286766559005768394338798750210
#define PI_OVER_2        1.570796326794896619231321691639751442098584699687552
#define PI_OVER_180      0.017453292519943295769236907684886127134428718885417

#define CHUNK_READ_COUNT 1000000

inline double minimaxsin(double x)
{
	while (x < 0.0)
		x += PI_TIMES_2;
		
	while (x >= PI_TIMES_2)
		x -= PI_TIMES_2;
		
	double s = 1.0;
	
	if (x >= PI)
	{
		x -= PI;
		s = -1.0;
	}
    // http://lolengine.net/blog/2011/12/21/better-function-approximations
    static const
    double a0 =  1.0,
           a1 = -1.666666666640169148537065260055e-1,
           a2 =  8.333333316490113523036717102793e-3,
           a3 = -1.984126600659171392655484413285e-4,
           a4 =  2.755690114917374804474016589137e-6,
           a5 = -2.502845227292692953118686710787e-8,
           a6 =  1.538730635926417598443354215485e-10;
    double x2 = x * x;
    return s * x * (a0 + x2 * (a1 + x2 * (a2 + x2
             * (a3 + x2 * (a4 + x2 * (a5 + x2 * a6))))));
}

uint64_t usec_diff(struct timeval start_time)
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return
		(uint64_t)((tv.tv_usec + tv.tv_sec*1e6) - 
			(start_time.tv_usec + start_time.tv_sec*1e6));
}

int main()
{
	double input;
	double sin, cos;
	
	char line [15];
	
	double * buffer;
	double * sin_buffer;
	double * cos_buffer;
	
	// do this in chunks of 1,000,000
	buffer = (double*) malloc (sizeof(double) * CHUNK_READ_COUNT);
	
	sin_buffer = (double*) malloc (sizeof(double) * CHUNK_READ_COUNT);
	cos_buffer = (double*) malloc (sizeof(double) * CHUNK_READ_COUNT);
	
	FILE *ifp = fopen("trig.in", "r");
	FILE *ofp = fopen("trig.out", "w");
	
	int do_stuff = 1;
	int last_read_count = 0;
	int i;
	
	struct timeval start_time;
	gettimeofday(&start_time,NULL);
	
	uint64_t usec_math_count = 0;
	uint64_t usec_total = 0;
	
	while (1)
	{
		memset (buffer, 0, sizeof(double) * CHUNK_READ_COUNT);
		memset (sin_buffer, 0, sizeof(double) * CHUNK_READ_COUNT);
		memset (cos_buffer, 0, sizeof(double) * CHUNK_READ_COUNT);
		
		last_read_count = 0;
		
		// read inputs to buffer
		for (i=0; i<CHUNK_READ_COUNT; i++)
		{
			do_stuff = fgets (line, 15, ifp) != (char*)0;
			if (!do_stuff)
			{
				break;
			}

			buffer[i] = atof(line);
		}
		
		last_read_count = i;
		
		// do math
		struct timeval math_time;
		gettimeofday(&math_time,NULL);
		
		for (i=0; i<last_read_count; i++)
		{
			sin_buffer[i] = minimaxsin(buffer[i] * PI_OVER_180);
			cos_buffer[i] = minimaxsin(PI_OVER_2 - buffer[i] * PI_OVER_180);
		}
		
		usec_math_count += usec_diff(math_time);
		
		// save latest output batch
		for (i=0; i<last_read_count; i++)
		{
			if (cos_buffer[i] < 0.000000001 && cos_buffer[i] > -0.000000001)
			{
				fprintf(ofp, "%.7f %.7f n\n", sin_buffer[i], cos_buffer[i]);
			}
			else
			{
				fprintf(ofp, "%.7f %.7f %.7f\n", sin_buffer[i], cos_buffer[i], (sin_buffer[i]/cos_buffer[i]));
			}
		}
		
		if (!do_stuff)
		{
			break;
		}
	}

	fclose(ifp);
	fclose(ofp);
	
	usec_total += usec_diff(start_time);
	
	printf("estimated math time in usec: %llu\n", usec_math_count);
	printf("estimated run time in usec: %llu\n", usec_total);

	return 0;
}