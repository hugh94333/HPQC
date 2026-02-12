#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function Prototypes
void initialise_mpi(int *argc, char ***argv);
void get_rank_and_size(int *rank, int *size);
void root_process(int world_size);
void worker_process(int my_rank, int world_size);
void finalise_mpi(void);

// Main Program
int main(int argc, char **argv)
{
    int my_rank = 0;
    int world_size = 0;

    initialise_mpi(&argc, &argv);
    get_rank_and_size(&my_rank, &world_size);

    if (world_size > 1)
    {
        if (my_rank == 0)
            root_process(world_size);
        else
            worker_process(my_rank, world_size);
    }
    else
    {
        printf("Unable to communicate with less than 2 processes. "
               "MPI communicator size = %d\n", world_size);
    }

    finalise_mpi();
    return 0;
}

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

// Root process receives four messages from each worker
void root_process(int world_size)
{
    int recv_message;
    int count = 1;
    int tag = 0;
    MPI_Status status;

    for (int source = 1; source < world_size; source++)
    {
        for (int i = 1; i <= 4; i++) // four messages per worker
        {
            MPI_Recv(&recv_message, count, MPI_INT, source, tag,
                     MPI_COMM_WORLD, &status);

            printf("Root received message %d: %d from Rank %d\n",
                   i, recv_message, source);
        }
    }
}

// Worker processes send four messages using different MPI send variants
void worker_process(int my_rank, int world_size)
{
    int count = 1;
    int tag = 0;
    int destination = 0;
    int message;
    MPI_Status status;
    MPI_Request request;
    double start_time, end_time;

    // Synchronous send
    message = my_rank * 1;
    start_time = MPI_Wtime();
    MPI_Ssend(&message, count, MPI_INT, destination, tag, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("Rank %d MPI_Ssend: %d, took %.6f s\n", my_rank, message, end_time - start_time);

    //Buffered send
    int bufsize = MPI_BSEND_OVERHEAD + sizeof(int);
    void *buffer = malloc(bufsize);
    MPI_Buffer_attach(buffer, bufsize);

    message = my_rank * 10;
    start_time = MPI_Wtime();
    MPI_Bsend(&message, count, MPI_INT, destination, tag, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("Rank %d MPI_Bsend: %d, took %.6f s\n", my_rank, message, end_time - start_time);

    MPI_Buffer_detach(&buffer, &bufsize);
    free(buffer);

    // Ready send
    message = my_rank * 100;
    start_time = MPI_Wtime();
    MPI_Rsend(&message, count, MPI_INT, destination, tag, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("Rank %d MPI_Rsend: %d, took %.6f s\n", my_rank, message, end_time - start_time);

    // non blocking send
    message = my_rank * 1000;
    start_time = MPI_Wtime();
    MPI_Isend(&message, count, MPI_INT, destination, tag, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);
    end_time = MPI_Wtime();
    printf("Rank %d MPI_Isend: %d, took %.6f s\n", my_rank, message, end_time - start_time);
}

// Finalise MPI environment
void finalise_mpi(void)
{
    MPI_Finalize();
}
