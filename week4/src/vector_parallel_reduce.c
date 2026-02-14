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

    long long global_sum;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    MPI_Reduce(&local_sum, &global_sum,
               1, MPI_LONG_LONG,
               MPI_SUM, 0, MPI_COMM_WORLD);

    double end = MPI_Wtime();

    if (rank == 0)
    {
        printf("Reduce Sum: %lld\n", global_sum);
        printf("Time: %f\n", end - start);
    }

    MPI_Finalize();
    return 0;
}
