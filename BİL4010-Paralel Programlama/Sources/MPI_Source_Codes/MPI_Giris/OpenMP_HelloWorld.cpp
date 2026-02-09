
// OpenMP program to print Hello World
// using C language

// OpenMP header
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int tid;

    /* Get the number of threads from command line */
    int thread_count = strtol(argv[1], NULL, 10);

    // Fork a team of threads giving them their own copies of variables 
    #pragma omp parallel num_threads(thread_count) private(tid)
    {
        /* Obtain thread number */
        tid = omp_get_thread_num();

        printf("Hello from thread %d \n", tid);

        /* Only master thread does this */

        if (tid == 0)
        {
            printf("Number of threads = %d \n", thread_count);
        }
    }
    return 0;
} /* main */


   
