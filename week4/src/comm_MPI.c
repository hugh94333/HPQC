#include <stdio.h>
#include <mpi.h>

/* =============================== */
/* Function Prototypes             */
/* =============================== */
void initialise_mpi(int *argc, char ***argv);
void get_rank_and_size(int *rank, int *size);
void root_process(int world_size);
void worker_process(int my_rank, int world_size);
void finalise_mpi(void);

/* =============================== */
/* Main Program                    */
/* =============================== */
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

/* =============================== */
/* Function Definitions            */
/* =============================== */

/* Initialise MPI environment */
void initialise_mpi(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
}

/* Obtain rank and communicator size */
void get_rank_and_size(int *rank, int *size)
{
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

/* Root process (rank 0) receives messages */
void root_process(int world_size)
{
    int recv_message;
    int count = 1;
    int tag = 0;
    MPI_Status status;

    for (int source = 1; source < world_size; source++)
    {
        MPI_Recv(&recv_message, count, MPI_INT,
                 source, tag, MPI_COMM_WORLD, &status);

        printf("Hello, I am 0 of %d. Received %d from Rank %d\n",
               world_size, recv_message, source);
    }
}

/* Worker processes (non-zero ranks) send messages */
void worker_process(int my_rank, int world_size)
{
    int send_message;
    int count = 1;
    int tag = 0;
    int destination = 0;

    send_message = my_rank;

    MPI_Send(&send_message, count, MPI_INT,
             destination, tag, MPI_COMM_WORLD);

    printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
           my_rank, world_size, send_message, destination);
}

/* Finalise MPI environment */
void finalise_mpi(void)
{
    MPI_Finalize();
}
