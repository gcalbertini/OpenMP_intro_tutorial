/*
**  PROGRAM: Mandelbrot area
**
**  PURPOSE: Program to compute the area of a  Mandelbrot set.
**           Correct answer should be around 1.510659.
**           WARNING: this program may contain errors
**
**  USAGE:   Program runs without input ... just run the executable
**            
**  HISTORY: Written:  (Mark Bull, August 2011).
**           Changed "comples" to "d_comples" to avoid collsion with 
**           math.h complex type (Tim Mattson, September 2011)
*/
// gcc -fopenmp mandel.c && ./a.out
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

# define NPOINTS 1000
# define MAXITER 1000

// also side note: make sure the struct is declared before function it calls for because otherwise there is no struct declared 
// in the argument associated with the function >> is then assumed to be something. Later after the struct is declared and the
// function is defined the compiler now uses the struct to associate to the function, but it has already done this process
// earlier with other associations therefore there is re-declaration >> warning from compiler
struct d_complex{
   double r;
   double i;
};
// cannot be void testpoint(void) as this would prompt threads from testpoint() to walk through global array at same time; pass in point we will test that is not from global scope
void testpoint(struct d_complex);


struct d_complex c;
int numoutside = 0;

int main(){
   int i, j;
   double area, error, eps  = 1.0e-5;


//   Loop over grid of points in the complex plane which contains the Mandelbrot set,
//   testing each point to see whether it is inside or outside the set.

//when setting default(none) we can see from compiler warning that j needs to be set to private as it is a *nested* variable, unlike i
// eps needs to be initialised with the global and be made firstprivate
#pragma omp parallel for default(shared) private(c,j) firstprivate(eps)
   for (i=0; i<NPOINTS; i++) {
     for (j=0; j<NPOINTS; j++) {
       c.r = -2.0+2.5*(double)(i)/(double)(NPOINTS)+eps;
       c.i = 1.125*(double)(j)/(double)(NPOINTS)+eps;
       //cannot simply be testpoint() as all threads are walking through array from global file scope at the same time!
       testpoint(c);
     }
   }

// Calculate area of set and error estimate and output the results
   
area=2.0*2.5*1.125*(double)(NPOINTS*NPOINTS-numoutside)/(double)(NPOINTS*NPOINTS);
   error=area/(double)NPOINTS;

   printf("Area of Mandlebrot set = %12.8f +/- %12.8f\n",area,error);
   printf("Correct answer should be around 1.510659\n");

}

void testpoint(struct d_complex c){

// Does the iteration z=z*z+c, until |z| > 2 when point is known to be outside set
// If loop count reaches MAXITER, point is considered to be inside the set

       struct d_complex z;
       int iter;
       double temp;

       z=c;
       for (iter=0; iter<MAXITER; iter++){
         temp = (z.r*z.r)-(z.i*z.i)+c.r;
         z.i = z.r*z.i*2+c.i;
         z.r = temp;
         if ((z.r*z.r+z.i*z.i)>4.0) {
           // avoid threads thrashing each other from incrementing one another's increments
           #pragma omp atomic
           numoutside++;
           break;
         }
       }

}

