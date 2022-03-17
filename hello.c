#include <stdio.h>
#include <omp.h>
//gcc -fopenmp hello.c && ./a.out
int main ()  
{
  #pragma omp parallel
  {
    int ID = omp_get_thread_num();
    printf("Hello(%d)",ID);
    printf("World(%d)\n",ID);
  }
}
