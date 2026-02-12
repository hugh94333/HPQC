#include <stdio.h>
#include <mpi.h>

void initialise_mpi(int *argc, char ***argv);
void get_rank_and_size(int *rank, int *size);
void worker_process(int my_rank, int world_size);
void finalise_mpi(void);

// Main Program
int main(int argc, char **argv)
{
    int my_rank = 0;
    int world_size = 0;

    // Initialise MPI
    initialise_mpi(&argc, &argv);

    // Get rank and size
    get_rank_and_size(&my_rank, &world_size);

    int recv_message, count, source, tag;
    recv_message = source = tag = 0;
    count = 1;
    MPI_Status status;

    if (world_size > 1)
    {
        if (my_rank == 0)
        {
            // Root processing
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
            // Worker logic moved to function
            worker_process(my_rank, world_size);
        }
    }
    else
    {
        printf("Unable to communicate with less than 2 processes. "
               "MPI communicator size = %d\n", world_size);
    }

    finalise_mpi();
    return 0;
}

// Function definitions

void initialise_mpi(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
}

void get_rank_and_size(int *rank, int *size)
{
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

// Worker process (non-root ranks)
void worker_process(int my_rank, int world_size)
{
    int send_message;
    int count = 1;
    int tag = 0;
    int destination = 0;

    send_message = my_rank *1;

    MPI_Send(&send_message, count, MPI_INT,
             destination, tag, MPI_COMM_WORLD);

    printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
           my_rank, world_size, send_message, destination);
}

void finalise_mpi(void)
{
    MPI_Finalize();
}
