/*

This program will numerically compute the integral of

				  4/(1+x*x)

from 0 to 1.  The value of this integral is pi -- which
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

*/
// gcc -fopenmp pi_no_false_sharing.c && ./a.out
#include <stdio.h>
#include <omp.h>
#define NUM_THREADS 4
// In this version we set pad to be number of double precision words in cache line;
// First element is id and second element always 0 so now guarantee that every
// lement of array sits on different cache line!
#define PAD 8 // assume 64 byte L1 cache line size
static long num_steps = 100000000;
double step;
int main()
{
	int i, nthreads;
	double x, pi;
	double start_time, run_time, sum[NUM_THREADS][PAD];
	step = 1.0 / (double)num_steps;
	omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
	{
		// These vars sit on stack for each thread
		int i, id, nthrds;
		double x;
		id = omp_get_thread_num();
		nthrds = omp_get_num_threads();
		// Pick a thread to save a copy of number of threads as
		// we simply request # of threads outside // region but
		// can be given fewer by env; have to check threads you got!
		// Copy num threads we had outside // region into it so
		// you know how many there were once you get out of it
		if (id == 0)
			nthreads = nthrds;

		start_time = omp_get_wtime();
		for (i = id, sum[id][0] = 0.0; i < num_steps; i += nthrds)
		{
			x = (i + 0.5) * step;
			sum[id][0] += 4.0 / (1.0 + x * x);
		}
	}
	// Now all vars from thread stack are lost (aside from master threads')
	// aside from sum which is shared to avoid race conditions
	for (i = 0, pi = 0.0; i < nthreads; ++i)
		pi += sum[i][0] * step;
	run_time = omp_get_wtime() - start_time;
	printf("\n pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi, run_time);
}
