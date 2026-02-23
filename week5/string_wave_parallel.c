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

//Functions
ProgramOptions check_args(int argc, char **argv);
double driver(double time);
void update_positions(double* local_positions, int local_size,
                      double left_val, double time);
void print_header(FILE* out_file, int points);

//Main
int main(int argc, char **argv)
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    ProgramOptions opts;

    if(rank == 0)
        opts = check_args(argc, argv);

    MPI_Bcast(&opts, sizeof(ProgramOptions), MPI_BYTE, 0, MPI_COMM_WORLD);

    int points = opts.points;
    int cycles = opts.cycles;
    int samples = opts.samples;
    int time_steps = cycles * samples + 1;
    double step_size = 1.0 / samples;

    //Timing Variables
    double total_start, total_end;
    double comp_time = 0.0;
    double comm_time = 0.0;
    double gather_time = 0.0;
    double write_time = 0.0;

    int base = points / size;
    int remainder = points % size;

    int local_size = base + (rank < remainder ? 1 : 0);

    int* recvcounts = NULL;
    int* displs = NULL;

    if(rank == 0) {
        recvcounts = malloc(size * sizeof(int));
        displs = malloc(size * sizeof(int));
        for(int i=0; i<size; i++) {
            recvcounts[i] = base + (i < remainder ? 1 : 0);
            displs[i] = i * base + (i < remainder ? i : remainder);
        }
    }

    double* local_positions = calloc(local_size, sizeof(double));
    double* global_positions = NULL;

    if(rank == 0)
        global_positions = malloc(points * sizeof(double));

    FILE* out_file = NULL;

    if(rank == 0) {
        out_file = fopen(opts.output_file, "w");
        if(!out_file) {
            fprintf(stderr, "Error opening output file.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        print_header(out_file, points);
    }

    //Simulation
    total_start = MPI_Wtime();

    for(int t = 0; t < time_steps; t++)
    {
        double time = t * step_size;
        double left_val = 0.0;
        double t0;

        //Communication
        t0 = MPI_Wtime();

        if(rank > 0)
            MPI_Recv(&left_val, 1, MPI_DOUBLE, rank-1, 0,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(rank < size-1)
            MPI_Send(&local_positions[local_size-1], 1,
                     MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD);

        comm_time += MPI_Wtime() - t0;

        //Computation
        t0 = MPI_Wtime();

        update_positions(local_positions, local_size, left_val, time);

        comp_time += MPI_Wtime() - t0;

        //Gather
        t0 = MPI_Wtime();

        MPI_Gatherv(local_positions, local_size, MPI_DOUBLE,
                    global_positions, recvcounts, displs,
                    MPI_DOUBLE, 0, MPI_COMM_WORLD);

        gather_time += MPI_Wtime() - t0;

        
        if(rank == 0) {
            t0 = MPI_Wtime();

            fprintf(out_file, "%d, %f", t, time);
            for(int i=0; i<points; i++)
                fprintf(out_file, ", %f", global_positions[i]);
            fprintf(out_file, "\n");

            write_time += MPI_Wtime() - t0;
        }
    }

    total_end = MPI_Wtime();
    double total_time = total_end - total_start;

    //Timing Results
    double max_comp, max_comm, max_gather, max_write, max_total;

    MPI_Reduce(&comp_time, &max_comp, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&comm_time, &max_comm, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&gather_time, &max_gather, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&write_time, &max_write, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&total_time, &max_total, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if(rank == 0) {
        printf("\n===== TIMING RESULTS =====\n");
        printf("Total runtime:      %f seconds\n", max_total);
        printf("Computation time:   %f seconds\n", max_comp);
        printf("Communication time: %f seconds\n", max_comm);
        printf("Gather time:        %f seconds\n", max_gather);
        printf("Write time:         %f seconds\n", max_write);
        printf("==========================\n");
    }


    if(rank == 0) {
        fclose(out_file);
        free(global_positions);
        free(recvcounts);
        free(displs);
    }

    free(local_positions);

    MPI_Finalize();
    return 0;
}

//checks

ProgramOptions check_args(int argc, char **argv)
{
    if(argc != 5) {
        fprintf(stderr,
        "Usage: %s <points> <cycles> <samples> <output_file>\n",
        argv[0]);
        exit(1);
    }

    ProgramOptions opts;
    opts.points = atoi(argv[1]);
    opts.cycles = atoi(argv[2]);
    opts.samples = atoi(argv[3]);
    strcpy(opts.output_file, argv[4]);

    if(opts.points <= 0 || opts.cycles <= 0 || opts.samples <= 0) {
        fprintf(stderr, "All numeric arguments must be positive.\n");
        exit(1);
    }

    return opts;
}

double driver(double time)
{
    return sin(2.0 * M_PI * time);
}

void update_positions(double* local_positions,
                      int local_size,
                      double left_val,
                      double time)
{
    double prev;

    if(left_val != 0.0)
        prev = left_val;
    else
        prev = driver(time);

    for(int i = 0; i < local_size; i++) {
        double temp = local_positions[i];
        local_positions[i] = prev;
        prev = temp;
    }
}

void print_header(FILE* out_file, int points)
{
    fprintf(out_file, "#, time");
    for(int i=0; i<points; i++)
        fprintf(out_file, ", y[%d]", i);
    fprintf(out_file, "\n");
}
