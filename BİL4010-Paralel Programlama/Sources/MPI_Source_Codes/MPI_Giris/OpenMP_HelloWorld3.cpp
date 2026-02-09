#define _CRT_SECURE_NO_DEPRECATE
// OpenMP program to print Hello World
// using C language

// OpenMP header
#include <omp.h>

#include <stdio.h>
#include <stdlib.h>

void Hello(void); /*Thread function*/

int main(int argc, char* argv[])
{
    /* Get the number of threads from command line */
    int thread_count = 1;
    
    printf("How many threads do you want to create: ");
    scanf("%d", &thread_count);

    omp_set_num_threads(thread_count); // kullanıcıdan kaç thread oluşturulması istenir


    // Beginning of parallel region
    #pragma omp parallel num_threads(thread_count)

    Hello();

    return 0;
} /* main */

void Hello(void)
{
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    printf("Hello from thread %d of %d\n", my_rank, thread_count);
}

