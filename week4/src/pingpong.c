#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Function prototypes
void initialise_mpi(int *argc, char ***argv);
void finalise_mpi(void);
void pingpong_root(int num_pings);
void pingpong_client(int num_pings);

// Main function
int main(int argc, char **argv)
{
    int my_rank, world_size;

    initialise_mpi(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 2) {
        if (my_rank == 0)
            printf("Ping-pong test requires exactly 2 MPI processes.\n");
        finalise_mpi();
        return 1;
    }

    if (argc < 2) {
        if (my_rank == 0)
            printf("Usage: mpirun -np 2 ./pingpong <num_pings>\n");
        finalise_mpi();
        return 1;
    }

    int num_pings = atoi(argv[1]);

    if (my_rank == 0)
        pingpong_root(num_pings);
    else
        pingpong_client(num_pings);

    finalise_mpi();
    return 0;
}

// Initialise MPI
void initialise_mpi(int *argc, char ***argv)
{
    MPI_Init(argc, argv);
}

// Finalise MPI
void finalise_mpi(void)
{
    MPI_Finalize();
}

// Root process: sends counter and receives it back 
void pingpong_root(int num_pings)
{
    int counter = 0;
    int tag = 0;
    int client_rank = 1;
    MPI_Status status;

    double start_time = MPI_Wtime();

    while (counter < num_pings) {
        // Send counter to client ("ping")
        MPI_Send(&counter, 1, MPI_INT, client_rank, tag, MPI_COMM_WORLD);

        // Receive counter back from client ("pong")
        MPI_Recv(&counter, 1, MPI_INT, client_rank, tag, MPI_COMM_WORLD, &status);
    }

    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;
    double average_time = elapsed_time / num_pings;

    printf("Ping-pong finished: %d pings\n", num_pings);
    printf("Total elapsed time: %.6f s\n", elapsed_time);
    printf("Average time per ping: %.6f s\n", average_time);
}

// Client process: receives counter, increments, and sends back 
void pingpong_client(int num_pings)
{
    int counter;
    int tag = 0;
    int root_rank = 0;
    MPI_Status status;

    for (int i = 0; i < num_pings; i++) {
        // Receive counter from root
        MPI_Recv(&counter, 1, MPI_INT, root_rank, tag, MPI_COMM_WORLD, &status);

        // Increment counter
        counter++;

        // Send counter back to root
        MPI_Send(&counter, 1, MPI_INT, root_rank, tag, MPI_COMM_WORLD);
    }
}
