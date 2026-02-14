#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = atoi(argv[1]);
    int chunk = n / size;
    int *local_array = malloc(chunk * sizeof(int));

    if (rank == 0)
    {
        int *array = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
            array[i] = i * (i + 1);

        for (int p = 1; p < size; p++)
            MPI_Send(array + p*chunk, chunk, MPI_INT, p, 0, MPI_COMM_WORLD);

        for (int i = 0; i < chunk; i++)
            local_array[i] = array[i];

        free(array);
    }
    else
    {
        MPI_Recv(local_array, chunk, MPI_INT,
                 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    long long local_sum = 0;
    for (int i = 0; i < chunk; i++)
        local_sum += local_array[i];

    long long global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1,
               MPI_LONG_LONG, MPI_SUM, 0,
               MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0)
    {
        printf("DIY Send/Recv Sum: %lld\n", global_sum);
        printf("Time: %f seconds\n", end - start);
    }

    free(local_array);
    MPI_Finalize();
    return 0;
}
