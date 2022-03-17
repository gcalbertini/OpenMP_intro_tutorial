/*

This program will numerically compute the integral of

                  4/(1+x*x)

from 0 to 1.  The value of this integral is pi -- which
is great since it gives us an easy way to check the answer.

The is the original sequential program.  It uses the timer
from the OpenMP runtime library

History: Written by Tim Mattson, 11/99.

*/
// gcc -fopenmp pi_atomic.c && ./a.out
#include <stdio.h>
#include <omp.h>
#define NUM_THREADS 4
// In this version we use synchronization to get around the
// fact that different machines have different cache line sizes
// and use atomic as a lightweight and efficient way to do 
// mutual exclusion
static long num_steps = 100000000;
double step;
int main()
{
    int nthreads;
    double pi = 0.0;
    double start_time, run_time;
    step = 1.0 / (double)num_steps;
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
    {
        // These vars sit on stack for each thread; now each
        // thread has own copy of sum in its stack
        int i, id, nthrds;
        double x, sum;
        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();
        // Pick a thread to save a copy of number of threads as
        // we simply request # of threads outside // region but
        // can be given fewer by env; have to check threads you got!
        // Copy num threads we had outside // region into it so
        // you know how many there were once you get out of it
        if (id == 0)
            nthreads = nthrds;

        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();

        start_time = omp_get_wtime();
        for (i = id, sum = 0.0; i < num_steps; i += nthrds)
        {
            x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
            /* Note: had we put in something like:
                #pragma omp critical
                {
                    pi += 4.0 / (1.0 + x * x);
                }
                } out of // region
                pi *= step;
                }

                We would've serialized the program as there is very little time computing x
                so most of time is spent inside the loop where all the threads are doing a computation
                Expect performance to tank -- put after the loop!
            */
        }
        sum *= step;
        #pragma omp atomic
            pi += sum;
    }
    run_time = omp_get_wtime() - start_time;
    printf("\n pi with %ld steps is %lf in %lf seconds\n ", num_steps, pi, run_time);
}
