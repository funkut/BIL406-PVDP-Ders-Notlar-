
#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char *argv[]) {

      int tid;
      int thread_count = strtol(argv[1], NULL, 10);
      
            

    /* Fork a team of threads giving them their own copies of variables */
#pragma omp parallel num_threads(thread_count) private (tid)
    {

        /* Obtain thread number */
        tid = omp_get_thread_num();
        printf("Hello World from thread = %d\n", tid);

        /* Only master thread does this */
        if (tid == 0)
        {           
            printf("Number of threads = %d\n", thread_count);
        }

    }  /* All threads join master thread and disband */

}
