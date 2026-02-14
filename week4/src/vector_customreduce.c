#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Custom sum function
void my_sum(void *invec, void *inoutvec,
            int *len, MPI_Datatype *datatype)
{
    long long *in = (long long*) invec;
    long long *inout = (long long*) inoutvec;

    for (int i = 0; i < *len; i++)
        inout[i] += in[i];
}

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

    // Compute local sum
    long long local_sum = 0;
    for (int i = rank; i < n; i += size)
        local_sum += i * (i + 1);

    long long global_sum_custom;
    long long global_sum_builtin;

    // Create custom MPI operation
    MPI_Op my_op;
    MPI_Op_create(my_sum, 1, &my_op);

    MPI_Barrier(MPI_COMM_WORLD);
    double start_custom = MPI_Wtime();

    MPI_Reduce(&local_sum, &global_sum_custom,
               1, MPI_LONG_LONG,
               my_op, 0, MPI_COMM_WORLD);

    double end_custom = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    double start_builtin = MPI_Wtime();

    MPI_Reduce(&local_sum, &global_sum_builtin,
               1, MPI_LONG_LONG,
               MPI_SUM, 0, MPI_COMM_WORLD);

    double end_builtin = MPI_Wtime();

    if (rank == 0)
    {
        printf("Custom Reduce Sum:   %lld\n", global_sum_custom);
        printf("Custom Time:         %f seconds\n", end_custom - start_custom);

        printf("Built-in Reduce Sum: %lld\n", global_sum_builtin);
        printf("Built-in Time:       %f seconds\n", end_builtin - start_builtin);

        if (global_sum_custom == global_sum_builtin)
            printf("Results MATCH ✅\n");
        else
            printf("Results DO NOT MATCH ❌\n");
    }

    MPI_Op_free(&my_op);
    MPI_Finalize();
    return 0;
}
