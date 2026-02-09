/* File:    mpi_trap.c
 *
 * Purpose: Implement parallel trapezoidal rule allowing user input
 *          of data
 *
 * Input:   a, b, n
 * Output:  Estimate of the area between x = a, x = b, x-axis,
 *          and graph of f(x) using the trapezoidal rule and n
 *          trapezoids.
 *
 * Compile: mpicc -g -Wall -o mpi_trap mpi_trap.c
 * Run:     mpiexec -n <number of processes> ./mpi_trap
 *
 * Algorithm:
 *    0.  Process 0 reads in a, b, and n, and distributes them
 *        among the processes.
 *    1.  Each process calculates "its" subinterval, i.e., its
 *        local a and b.
 *    2.  Each process estimates the area of f(x)
 *        over its subinterval using the trapezoidal rule.
 *    3a. Each process != 0 sends its area to 0.
 *    3b. Process 0 sums the calculations received from
 *        the individual processes and prints the result.
 *
 * Note:  f(x) is hardwired.
 */

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>

 /* We'll be using MPI routines, definitions, etc. */
#include <mpi.h>

void Get_data(int p, int my_rank, double* a_p, double* b_p, int* n_p);

double Trap(double local_a, double local_b, int local_n,
    double h);    /* Calculate local area  */

double f(double x); /* function we're integrating */


int main(int argc, char** argv) {
    int         my_rank;   /* My process rank           */
    int         p;         /* The number of processes   */
    double      a;         /* Left endpoint             */
    double      b;         /* Right endpoint            */
    int         n;         /* Number of trapezoids      */
    double      h;         /* Trapezoid base length     */
    double      local_a;   /* Left endpoint my process  */
    double      local_b;   /* Right endpoint my process */
    int         local_n;   /* Number of trapezoids for  */
    /* my calculation            */
    double      my_area;   /* Integral over my interval */
    double      total;     /* Total area                */
    int         source;    /* Process sending area      */
    int         dest = 0;  /* All messages go to 0      */
    int         tag = 0;
    MPI_Status  status;

    /* Let the system do what it needs to start up MPI */
    MPI_Init(&argc, &argv);

    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    
    Get_data(p, my_rank, &a, &b, &n);

    h = (b - a) / n;    /* h is the same for all processes */
    local_n = n / p;  /* So is the number of trapezoids */

    /* Length of each process' interval of
     * integration = local_n*h.  So my interval
     * starts at: */
    local_a = a + my_rank * local_n * h;
    local_b = local_a + local_n * h;
    my_area = Trap(local_a, local_b, local_n, h);

    /* Add up the areas calculated by each process */
    if (my_rank == 0) {
        total = my_area;
        for (source = 1; source < p; source++) {
            MPI_Recv(&my_area, 1, MPI_DOUBLE, source, tag,
                MPI_COMM_WORLD, &status);
            total = total + my_area;
        }
    }
    else {
        MPI_Send(&my_area, 1, MPI_DOUBLE, dest,
            tag, MPI_COMM_WORLD);
    }

    /* Print the result */
    if (my_rank == 0) {
        printf("With n = %d trapezoids, our estimate\n",
            n);
        printf("of the area from %f to %f = %.15f\n",
            a, b, total);
    }

    /* Shut down MPI */
    MPI_Finalize();

    return 0;
} /*  main  */

/*------------------------------------------------------------------
 * Function:     Get_data
 * Purpose:      Read in the data on process 0 and send to other
 *               processes
 * Input args:   p, my_rank
 * Output args:  a_p, b_p, n_p
 */
void Get_data(int p, int my_rank, double* a_p, double* b_p, int* n_p) {
    int        q;
    MPI_Status status;
    

    if (my_rank == 0) {
        
        printf("Enter a, b, and n\n");  
        fflush(stdout);
        scanf("%lf %lf %d", a_p, b_p, n_p);
        

        for (q = 1; q < p; q++) {
            MPI_Send(a_p, 1, MPI_DOUBLE, q, 0, MPI_COMM_WORLD);
            MPI_Send(b_p, 1, MPI_DOUBLE, q, 0, MPI_COMM_WORLD);
            MPI_Send(n_p, 1, MPI_INT, q, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(a_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(b_p, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(n_p, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
}  /* Get_data */

/*------------------------------------------------------------------
 * Function:     Trap
 * Purpose:      Estimate a definite area using the trapezoidal
 *               rule
 * Input args:   local_a (my left endpoint)
 *               local_b (my right endpoint)
 *               local_n (my number of trapezoids)
 *               h (stepsize = length of base of trapezoids)
 * Return val:   Trapezoidal rule estimate of area from
 *               local_a to local_b
 */
double Trap(
    double  local_a   /* in */,
    double  local_b   /* in */,
    int     local_n   /* in */,
    double  h         /* in */) {
    double my_area;   /* Store my result in my_area  */
    double x;
    int i;

    my_area = (f(local_a) + f(local_b)) / 2.0;
    x = local_a;
    for (i = 1; i <= local_n - 1; i++) {
        x = local_a + i * h;
        my_area = my_area + f(x);
    }
    my_area = my_area * h;

    return my_area;
} /*  Trap  */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 */
double f(double x) {
    double return_val;

    return_val = x * x + 1.0;

    return return_val;
} /* f */

