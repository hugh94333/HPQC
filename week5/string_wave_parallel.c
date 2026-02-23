#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

typedef struct {
    int points;
    int cycles;
    int samples;
    char output_file[256];
} ProgramOptions;

//Function Declarations
ProgramOptions check_args(int argc, char **argv);
double driver(double time);
void update_positions(double* local_positions, int local_size, double left_val, double* new_left_val, double time);
void print_header(FILE* out_file, int points);

//Main Function
int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ProgramOptions opts;
    if(rank == 0) {
        opts = check_args(argc, argv);
    }
    // Broadcast options to all ranks
    MPI_Bcast(&opts, sizeof(ProgramOptions), MPI_BYTE, 0, MPI_COMM_WORLD);

    int points = opts.points;
    int cycles = opts.cycles;
    int samples = opts.samples;
    int time_steps = cycles * samples + 1;
    double step_size = 1.0 / samples;

    // Determine chunk for each process
    int base_points = points / size;
    int remainder = points % size;
    int local_size = base_points + (rank < remainder ? 1 : 0);

    int offset = rank * base_points + (rank < remainder ? rank : remainder);

    double* local_positions = (double*) malloc(local_size * sizeof(double));
    double* global_positions = NULL;
    if(rank == 0)
        global_positions = (double*) malloc(points * sizeof(double));

    for(int i=0; i<local_size; i++)
        local_positions[i] = 0.0;

    double* time_stamps = (double*) malloc(time_steps * sizeof(double));
    for(int t=0; t<time_steps; t++)
        time_stamps[t] = t * step_size;

    FILE* out_file = NULL;
    if(rank == 0) {
        out_file = fopen(opts.output_file, "w");
        if(!out_file) {
            fprintf(stderr, "Could not open output file: %s\n", opts.output_file);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        print_header(out_file, points);
    }

    //Main Time Loop
    for(int t=0; t<time_steps; t++)
    {
        double left_val = 0.0, right_val = 0.0;
        double new_left_val = 0.0;

        // Send receive boundary values with neighbors
        if(rank > 0) {
            MPI_Recv(&left_val, 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if(rank < size-1) {
            MPI_Send(&local_positions[local_size-1], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);
        }

        // Update local positions
        update_positions(local_positions, local_size, left_val, &new_left_val, time_stamps[t]);

        // Send new first element to previous rank
        if(rank > 0) {
            MPI_Send(&local_positions[0], 1, MPI_DOUBLE, rank-1, 1, MPI_COMM_WORLD);
        }
        if(rank < size-1) {
            MPI_Recv(&right_val, 1, MPI_DOUBLE, rank+1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        // Gather all positions to rank 0
        int* recvcounts = NULL;
        int* displs = NULL;
        if(rank == 0) {
            recvcounts = (int*) malloc(size * sizeof(int));
            displs = (int*) malloc(size * sizeof(int));
            for(int p=0; p<size; p++) {
                recvcounts[p] = points / size + (p < remainder ? 1 : 0);
                displs[p] = p * (points / size) + (p < remainder ? p : remainder);
            }
        }

        MPI_Gatherv(local_positions, local_size, MPI_DOUBLE,
                    global_positions, recvcounts, displs, MPI_DOUBLE,
                    0, MPI_COMM_WORLD);

        // Rank 0 writes CSV
        if(rank == 0) {
            fprintf(out_file, "%d, %lf", t, time_stamps[t]);
            for(int i=0; i<points; i++)
                fprintf(out_file, ", %lf", global_positions[i]);
            fprintf(out_file, "\n");
        }

        if(rank == 0) {
            free(recvcounts);
            free(displs);
        }
    }

    //Cleanup
    if(rank == 0) fclose(out_file);
    free(local_positions);
    if(rank == 0) free(global_positions);
    free(time_stamps);

    MPI_Finalize();
    return 0;
}

//Functions
ProgramOptions check_args(int argc, char **argv)
{
    ProgramOptions opts;

    if(argc != 5) {
        fprintf(stderr, "Usage: %s <points> <cycles> <samples> <output_file>\n", argv[0]);
        exit(1);
    }

    opts.points = atoi(argv[1]);
    opts.cycles = atoi(argv[2]);
    opts.samples = atoi(argv[3]);
    strcpy(opts.output_file, argv[4]);

    if(opts.points <= 0 || opts.cycles <= 0 || opts.samples <= 0) {
        fprintf(stderr, "All numerical arguments must be positive.\n");
        exit(1);
    }

    return opts;
}

// Simple harmonic driver for first element
double driver(double time)
{
    return sin(time * 2.0 * M_PI);
}

// Update local positions "follow the leader"
void update_positions(double* local_positions, int local_size, double left_val, double* new_left_val, double time)
{
    double prev = local_positions[0];
    if(left_val != 0.0)
        local_positions[0] = left_val;
    else
        local_positions[0] = driver(time);

    for(int i=1; i<local_size; i++) {
        double temp = local_positions[i];
        local_positions[i] = prev;
        prev = temp;
    }

    *new_left_val = local_positions[local_size-1];
}

// Print CSV header
void print_header(FILE* out_file, int points)
{
    fprintf(out_file, "#, time");
    for(int i=0; i<points; i++)
        fprintf(out_file, ", y[%d]", i);
    fprintf(out_file, "\n");
}
