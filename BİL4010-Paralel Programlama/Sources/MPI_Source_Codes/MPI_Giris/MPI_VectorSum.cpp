/* parallel_dot.c -- compute a dot product of a vector distributed among
 *     the processes.  Uses a block distribution of the vectors.
 *
 * Input:
 *     n: global order of vectors
 *     x, y:  the vectors
 *
 * Output:
 *     the dot product of x and y.
 *
 * Note:  Arrays containing vectors are statically allocated.  Assumes
 *     n, the global order of the vectors, is divisible by p, the number
 *     of processes.
 *
 */

#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "mpi.h"

#define MAXN 50000000
#define TUNIT 1.0e+6
double f(double);

main(int argc, char* argv[])
{
    double* x, * y, * local_x, * local_y;
    int    n_bar;  /* = n/p */
    double  dot, local_dot;
    int    p, my_rank, i, n;
    double start, finish, dt1;
    clock_t tstart, tend;
    double cpu_time_used;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        printf("Enter the number of elements in each vector: "); scanf("%d", &n);
        x = (double*)calloc(n, sizeof(double));
        y = (double*)calloc(n, sizeof(double));
        for (i = 0; i < n; i++) { x[i] = 1.0;  y[i] = 1.0; }
    }
    if (my_rank == 0) {
        start = MPI_Wtime();
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n_bar = n / p;
    local_x = (double*)calloc(n_bar, sizeof(double));
    local_y = (double*)calloc(n_bar, sizeof(double));

    MPI_Scatter(x, n_bar, MPI_DOUBLE, local_x, n_bar, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(y, n_bar, MPI_DOUBLE, local_y, n_bar, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /******* calculate my local dot product */
    local_dot = 0.0;
    for (i = 0; i < n_bar; i++)
        local_dot += f(local_x[i]) * f(local_y[i]);
    free(local_x); free(local_y);

    /******* calculate the total            */
    MPI_Reduce(&local_dot, &dot, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        finish = MPI_Wtime();
    }

    if (my_rank == 0) {
        cpu_time_used = ((double)(tend - tstart)) / CLOCKS_PER_SEC;
        printf("Local cpu time : %12.3f secs\n\n", cpu_time_used);
        printf("The dot product is %f\n GTOD_time = %12.3f secs\n", dot, dt1 / TUNIT);
        printf("WTime_time = %12.3f secs\n", finish - start);
    }
    MPI_Finalize();
}  /* main */

double f(double x)
{
    /* return (double)(log(exp(sin(M_PI*x/2.0))));*/
    return x;
    /*  return sinh(atan(cbrt(log(exp(pow(tan(asinh(x)),3.0))))));*/
}