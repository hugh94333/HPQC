#include <stdio.h>
#include <mpi.h>

void initialise_mpi(int *argc, char ***argv);
void finalise_mpi(void);

// Main Program

int main(int argc, char **argv)
{
    // MPI environment setup 
    initialise_mpi(&argc, &argv);

    printf("MPI initialized successfully.\n");

    // Finalise MPI 
    finalise_mpi();

    return 0;
}

// Function Definitions
// Initialise MPI environment 
void initialise_mpi(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
}

// Finalise MPI environment
void finalise_mpi(void)
{
    MPI_Finalize();
}

