#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    int my_rank, num_procs;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Get the total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // Print Hello World message
    printf("Hello World! %d of %d\n", my_rank, num_procs);

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}
