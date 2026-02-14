#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2)
    {
        if (rank == 0)
            printf("Usage: %s VECTOR_SIZE\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int n = atoi(argv[1]);
    int *array = malloc(n * sizeof(int));

    if (rank == 0)
    {
        for (int i = 0; i < n; i++)
            array[i] = i * (i + 1);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Bcast(array, n, MPI_INT, 0, MPI_COMM_WORLD);

    long long local_sum = 0;
    for (int i = rank; i < n; i += size)
        local_sum += *(array + i);

    long long global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_LONG,
               MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0)
    {
        printf("Broadcast Sum: %lld\n", global_sum);
        printf("Time: %f seconds\n", end - start);
    }

    free(array);
    MPI_Finalize();
    return 0;
}
