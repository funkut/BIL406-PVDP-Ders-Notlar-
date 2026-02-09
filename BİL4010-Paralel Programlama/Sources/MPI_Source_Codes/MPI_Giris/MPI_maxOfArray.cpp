#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <math.h>

#define RUNS 100

long* generate_random(size_t num, int rank);
long find_max(long* numbers, size_t size);

void start_find_max(int rank, int size, long* random_numbers,
    size_t num_per_proc, long* overall_max, double* elapsed);

int main(int argc, char** argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Barrier(MPI_COMM_WORLD);

    /* `num` is the total number of random numbers between all processes; allow
     * an optional parameter to set `num` */
    size_t num = 256000000;
    if (argc == 2)
        num = (size_t)atoi(argv[1]);

    /* generate some random numbers for `rank` */
    size_t num_per_proc = num / size;
    long* random_numbers = generate_random(num_per_proc, rank);
    if (rank == 0) {
        printf("Generated %zd random numbers per process\n", num_per_proc);
    }


    long overall_max;
    double elapsed = 0;
    double elapsed_total = 0;

    for (int i = 0; i < RUNS; i++) {

        start_find_max(rank, size, random_numbers, num_per_proc, &overall_max, &elapsed);
        
        if (rank == 0) {
            elapsed_total += elapsed;
            printf("Took %lf seconds (sum = %lf)\n", elapsed, elapsed_total);
        }
    }

    if (rank == 0) {
        elapsed = elapsed_total / RUNS;
        printf("Max is %ld; that took %lf seconds (on average over %d runs).\n",
            overall_max, elapsed, RUNS);
    }

    free(random_numbers);
    MPI_Finalize();
    return 0;
}

/* Generate `num` random nunmbers and returns a pointer to the first number;
 * the caller is responsible for freeing */
long* generate_random(size_t num, int rank)
{
    // seed the random number generator; add the rank to the time to make sure
    // every process has a different seed
    srand(time(NULL) + rank);

    long* random_numbers = (long*)malloc(sizeof(long) * num);
    if (!random_numbers) { printf("%d Failed to malloc\n", rank); fflush(stdout); exit(EXIT_FAILURE); }

    for (size_t i = 0; i < num; i++) {
        random_numbers[i] = rand();
    }
    return random_numbers;
}


/* return the largest number in `numbers`, an array of size `size`;
 * if size == 0, returns LONG_MIN */
long find_max(long* numbers, size_t size)
{
    if (size > 0) {
        long max = numbers[0];
        for (size_t i = 1; i < size; i++) {
            if (numbers[i] > max)
                max = numbers[i];
        }
        return max;
    }
    return LONG_MIN;
}

/*
 * rank - the current process
 * size - the total number of processes
 * random_numbers - the set of numbers of the current process to find the max of
 * num_per_proc - the length of `random_numbers` array
 * overall_max - rank 0 will write the max to this long
 * elapsed - rank 0 will write the number of seconds taken
 *
 * Find the max among all processes, each having a different `random_numbers`
 */
void start_find_max(int rank, int size, long* random_numbers,
    size_t num_per_proc, long* overall_max, double* elapsed)
{
    MPI_Status status;

    double start = MPI_Wtime();

    long max = find_max(random_numbers, num_per_proc);

    int still_alive = 1;
    int level;

    /* `level` is the current level of the complete binary tree. At level 0,
     * there are n nodes (processes); each of these nodes has its max already,
     * computed by find_max. A node with label `rank` that is in an
     * even-numbered position on the current level becomes a parent on the next
     * level; its new max is the maximum of its current max and the max of node
     * `rank + 2^level`. A node with label `rank` in an odd-numbered position
     * on the current level is responsible for sending its max to the parent,
     * `rank - 2^level`; after that, the node becomes inactive (still_alive is
     * set to 0). After log2(size) levels have been created, node with rank 0
     * contains the maximum.
     *
     * Note that for the sequential version, rank == 1 and so this for loop
     * will be skipped.
     */

    for (level = 0; level < (int)log2(size); level++) {

        if (still_alive) {

            int position = rank / (int)pow(2, level);

            if (position % 2 == 0) {
                // I am a receiver
                long sender_max;
                int sending_rank = rank + (int)pow(2, level);

                MPI_Recv(&sender_max, 1, MPI_LONG, sending_rank, 0, MPI_COMM_WORLD, &status);

                if (sender_max > max) {
                    max = sender_max;
                }

            }
            else {
                // I am a sender
                int receiving_rank = rank - (int)pow(2, level);

                MPI_Send(&max, 1, MPI_LONG, receiving_rank, 0, MPI_COMM_WORLD);
                still_alive = 0;
            }

        }

        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0) printf("Finished level %d, %lf seconds so far.\n", level, MPI_Wtime() - start);
    }
    double end = MPI_Wtime();


    if (rank == 0) {
        *overall_max = max;
        *elapsed = end - start;
    }
}