#include <stdio.h>
#include <mpi.h>


void initialise_mpi(int *argc, char ***argv);
void get_rank_and_size(int *rank, int *size);
void finalise_mpi(void);


// Main Program
int main(int argc, char **argv)
{
    int my_rank = 0;
    int world_size = 0;

    // Initialise MPI
    initialise_mpi(&argc, &argv);

    // Get rank and communicator size
    get_rank_and_size(&my_rank, &world_size);

    // Original communication logic still in main

    int send_message, recv_message, count, dest, source, tag;
    send_message = recv_message = dest = source = tag = 0;
    count = 1;
    MPI_Status status;

    if (world_size > 1)
    {
        if (0 == my_rank)
        {
            for (int their_rank = 1; their_rank < world_size; their_rank++)
            {
                source = their_rank;

                MPI_Recv(&recv_message, count, MPI_INT,
                         source, tag, MPI_COMM_WORLD, &status);

                printf("Hello, I am %d of %d. Received %d from Rank %d\n",
                       my_rank, world_size, recv_message, source);
            }
        }
        else
        {
            dest = 0;
            send_message = my_rank;

            MPI_Send(&send_message, count, MPI_INT,
                     dest, tag, MPI_COMM_WORLD);

            printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
                   my_rank, world_size, send_message, dest);
        }
    }
    else
    {
        printf("Unable to communicate with less than 2 processes. "
               "MPI communicator size = %d\n", world_size);
    }

    // Finalise MPI
    finalise_mpi();

    return 0;
}

//
// Function Definitions

// Initialise MPI environment
void initialise_mpi(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
}

// Obtain rank and communicator size
void get_rank_and_size(int *rank, int *size)
{
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

// Finalise MPI environment
void finalise_mpi(void)
{
    MPI_Finalize();
}

