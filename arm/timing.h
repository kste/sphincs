#ifndef TIMING_H
#define TIMING_H

#include <sys/time.h>

#define CPU_SPEED 2.0e3
#define CPU_ID 0
#define NUM_TIMINGS 10000
#define ITERATIONS 1000



int compareDouble(const void *x, const void *y)
{
  double xx = *(double*)x, yy = *(double*)y;
  if (xx < yy) return -1;
  if (xx > yy) return  1;
  return 0;
}

unsigned long long int startTimer(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1e6 + tv.tv_usec) * CPU_SPEED; // usec
}

unsigned long long int endTimer(void)
{
    struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1e6 + tv.tv_usec) * CPU_SPEED; // usec
}

#endif
