#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <math.h>
#include "mpi.h"
#include <stdlib.h>

const int MAX = 10000;
int rank, size;
int i, j, count, perproc, minindex;
int n;
int A, B;
int temp;

int swap(int* x, int* y) {
    temp = *x;
    *x = *y;
    *y = temp;
    return 0;
}

int main(int argc, char** argv) {
    int arr[MAX];
    int value[MAX];
    MPI_Status status;
    //int  arr[]={16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1};
    // srand(time(0));
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    /*if(size != 16) {
      if(rank == 0) {
        printf("Please use 16 processors.\n");
      }
      MPI_Finalize;
      exit(1);
    }*/
    /*THIS VERSION GENERATES "MAX" RANDOM VALUES AND USES MPI_SCATTER
     * TO DISTRIBUTE THESE TO EACH PROCESS.  HERE, EACH PROCESS RECEIVES
     * ONE VALUE
    */

    if (rank == 0) {
        printf("How many numbers per processor?\n");
        fflush(stdout);
        scanf("%d", &perproc);
        printf("cpus: %d, #s per cpu: %d\n", size, perproc);
        fflush(stdout);
        printf("Initial array: ");
        fflush(stdout);
        for (i = 0; i < size * perproc; i++) {
            arr[i] = rand() % 100;
            printf("%d ", arr[i]);
        }
        printf("\n");
    }
    MPI_Bcast(&perproc, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(&arr, perproc, MPI_INT, &value, perproc, MPI_INT, 0, MPI_COMM_WORLD);
    //printf("Rank %d receives %d\n", rank, value[0]);
    for (n = 0; n < size; n++) {
        /*if(rank == 0) {
          printf("meep!\n");
        }*/
        if (n % 2 == 0) {
            if (rank % 2 == 0) {
                MPI_Send(&value[0], perproc, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&value[perproc], perproc, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
                for (i = 0; i < (perproc * 2 - 1); i++) {
                    minindex = i;
                    for (j = i + 1; j < perproc * 2; j++) {
                        if (value[j] < value[minindex]) {
                            minindex = j;
                        }
                    }
                    if (minindex > i) {
                        swap(&value[i], &value[minindex]);
                    }
                }
                //printf("Rank %d n: %d, %d\n", rank,n, value[0]);
            }
            else {
                MPI_Recv(&value[perproc], perproc, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
                MPI_Send(&value[0], perproc, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                for (i = 0; i < (perproc * 2 - 1); i++) {
                    minindex = i;
                    for (j = i + 1; j < perproc * 2; j++) {
                        if (value[j] < value[minindex]) {
                            minindex = j;
                        }
                    }
                    if (minindex > i) {
                        swap(&value[i], &value[minindex]);
                    }
                }
                for (i = 0; i < perproc; i++) {
                    swap(&value[i], &value[i + perproc]);
                }
                //printf("Rank %d n: %d, %d\n", rank,n, value[0]);
            }
        }
        else {
            if ((rank % 2 == 1) && (rank != (size - 1))) {
                MPI_Send(&value[0], perproc, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
                MPI_Recv(&value[perproc], perproc, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &status);
                for (i = 0; i < (perproc * 2 - 1); i++) {
                    minindex = i;
                    for (j = i + 1; j < perproc * 2; j++) {
                        if (value[j] < value[minindex]) {
                            minindex = j;
                        }
                    }
                    if (minindex > i) {
                        swap(&value[i], &value[minindex]);
                    }
                }
                //printf("Rank %d n: %d, %d\n", rank,n, value[0]);
            }
            else if (rank != 0 && rank != (size - 1)) {
                MPI_Recv(&value[perproc], perproc, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
                MPI_Send(&value[0], 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD);
                for (i = 0; i < (perproc * 2 - 1); i++) {
                    minindex = i;
                    for (j = i + 1; j < perproc * 2; j++) {
                        if (value[j] < value[minindex]) {
                            minindex = j;
                        }
                    }
                    if (minindex > i) {
                        swap(&value[i], &value[minindex]);
                    }
                }
                for (i = 0; i < perproc; i++) {
                    swap(&value[i], &value[i + perproc]);
                }
                //printf("Rank %d n: %d, %d\n", rank,n, value[0]);
            }
        }
    }
    //if(rank == 0) {
      //printf("%d: meow!\n", rank);
    //}		
    //MPI_Scatter(&arr,1,MPI_INT,&value, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&value[0], perproc, MPI_INT, &arr[0], perproc, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        printf("Sorted array: ");
        for (i = 0; i < size * perproc; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
    }
    MPI_Finalize();
    return 0;
}