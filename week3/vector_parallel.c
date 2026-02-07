#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// Function declarations
int check_args(int argc, char **argv);
int* initialise_vector(int size);
int sum_vector_serial(int vector[], int size);

int main(int argc, char **argv)
{
    // Get vector size from command line
    int num_arg = check_args(argc, argv);

    // Allocate and initialise vector
    int* my_vector = initialise_vector(num_arg);

    // Fill vector with non-trivial values (i*(i+1))
    for (int i = 0; i < num_arg; i++)
        my_vector[i] = i * (i + 1);

    // Start timing
    double start_time = omp_get_wtime();

    // Parallel sum using OpenMP
    long long sum = 0; // Use long long to avoid overflow
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < num_arg; i++)
    {
        sum += my_vector[i];
    }

    // End timing
    double end_time = omp_get_wtime();

    // Print results
    printf("Sum: %lld\n", sum);
    printf("Parallel CPU time: %f seconds\n", end_time - start_time);

    // Free memory
    free(my_vector);

    return 0;
}

// Function to initialise vector with zeros
int* initialise_vector(int size)
{
    int* vector = malloc(size * sizeof(int));
    if (vector == NULL)
    {
        printf("Memory allocation failed\n");
        exit(1);
    }
    for (int i = 0; i < size; i++)
        vector[i] = 0;
    return vector;
}

// Function to check command line arguments
int check_args(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [VECTOR_SIZE]\n", argv[0]);
        exit(1);
    }
    int num = atoi(argv[1]);
    if (num <= 0)
    {
        fprintf(stderr, "Vector size must be positive!\n");
        exit(1);
    }
    return num;
}
