
#include <stdio.h>
#include <string.h>  /* For strlen             */
#include <mpi.h>     /* For MPI functions, etc */

const int MAX_STRING = 100;

int main(void) {
    char       greeting[MAX_STRING];
    int        comm_sz; // Number of Process
    int        my_rank; // id of the Process

    /* Start up MPI */
    MPI_Init(NULL, NULL);

    /* Get the number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    /* Get my rank among all the processes */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank != 0) {
        sprintf_s(greeting, "%d Slave process'den merhaba, toplam %d process var!", my_rank, comm_sz);
        MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    else {      
        printf("%d nolu Master process'den selamlar, toplam %d process var!\n", my_rank, comm_sz);
        for (int q = 1; q < comm_sz; q++){
            MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s\n", greeting);
        }
        
    }
    /* Shut down MPI */
    MPI_Finalize();
    return 0;
}  /* main */