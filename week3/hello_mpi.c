#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    int my_rank = -1, uni_size = -1;

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

    printf("Hello, I am %d of %d\n", my_rank, uni_size);
    return 0;
}

