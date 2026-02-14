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

    long long local_sum = 0;
    for (int i = rank; i < n; i += size)
        local_sum += i * (i + 1);

    long long *all_sums = NULL;
    if (rank == 0)
        all_sums = malloc(size * sizeof(long long));

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Gather(&local_sum, 1, MPI_LONG_LONG,
               all_sums, 1, MPI_LONG_LONG,
               0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        long long global_sum = 0;
        for (int i = 0; i < size; i++)
            global_sum += all_sums[i];

        double end = MPI_Wtime();
        printf("Gather Sum: %lld\n", global_sum);
        printf("Time: %f\n", end - start);

        free(all_sums);
    }

    MPI_Finalize();
    return 0;
}
