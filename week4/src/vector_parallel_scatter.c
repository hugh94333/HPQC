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

    int *array = NULL;
    int *local_array = malloc(chunk * sizeof(int));

    if (rank == 0)
    {
        array = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++)
            array[i] = i * (i + 1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Scatter(array, chunk, MPI_INT,
                local_array, chunk, MPI_INT,
                0, MPI_COMM_WORLD);

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
        printf("Scatter Sum: %lld\n", global_sum);
        printf("Time: %f seconds\n", end - start);
    }

    free(local_array);
    if (rank == 0) free(array);

    MPI_Finalize();
    return 0;
}
