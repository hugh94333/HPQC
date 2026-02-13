#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define NUM_TESTS 80
#define MIN_SIZE 8
#define MAX_SIZE 2097152

void run_bandwidth_test(int num_pings, int my_rank);

int main(int argc, char **argv)
{
    int my_rank, world_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 2) {
        if (my_rank == 0)
            printf("This program requires exactly 2 MPI processes.\n");
        MPI_Finalize();
        return 1;
    }

    if (argc < 2) {
        if (my_rank == 0)
            printf("Usage: mpirun -np 2 ./pingpong_sweep <num_pings>\n");
        MPI_Finalize();
        return 1;
    }

    int num_pings = atoi(argv[1]);

    if (my_rank == 0)
        printf("Size(Bytes)\tAvgTime(s)\tBandwidth(MB/s)\n");

    run_bandwidth_test(num_pings, my_rank);

    MPI_Finalize();
    return 0;
}

void run_bandwidth_test(int num_pings, int my_rank)
{
    double log_min = log((double)MIN_SIZE);
    double log_max = log((double)MAX_SIZE);

    for (int test = 0; test < NUM_TESTS; test++)
    {
        double fraction = (double)test / (NUM_TESTS - 1);
        int message_size = (int)exp(log_min + fraction * (log_max - log_min));

        int num_ints = message_size / sizeof(int);
        if (num_ints < 1) num_ints = 1;

        int *buffer = malloc(num_ints * sizeof(int));
        if (!buffer) {
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        MPI_Status status;
        double start_time, end_time;

        if (my_rank == 0)
        {
            start_time = MPI_Wtime();

            for (int i = 0; i < num_pings; i++) {
                MPI_Send(buffer, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD);
                MPI_Recv(buffer, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            }

            end_time = MPI_Wtime();

            double elapsed = end_time - start_time;
            double avg_time = elapsed / num_pings;
            double total_bytes = (double)message_size * num_pings * 2;
            double bandwidth = total_bytes / elapsed / 1e6;

            printf("%d\t\t%.6e\t%.3f\n",
                   message_size, avg_time, bandwidth);
        }
        else
        {
            for (int i = 0; i < num_pings; i++) {
                MPI_Recv(buffer, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
                MPI_Send(buffer, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
        }

        free(buffer);
    }
}
