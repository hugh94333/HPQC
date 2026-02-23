#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef struct {
    int points;
    int cycles;
    int samples;
    char output_file[256];
} ProgramOptions;

ProgramOptions check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
void update_positions(double* positions, int points, double time);
double driver(double time);
void print_header(FILE* out_file, int points);

int main(int argc, char **argv)
{
    ProgramOptions opts = check_args(argc, argv);

    int points = opts.points;
    int cycles = opts.cycles;
    int samples = opts.samples;

    int time_steps = cycles * samples + 1;
    double step_size = 1.0 / samples;

    double* time_stamps = malloc(time_steps * sizeof(double));
    double* positions   = malloc(points * sizeof(double));

    if (!time_stamps || !positions) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    initialise_vector(time_stamps, time_steps, 0.0);
    initialise_vector(positions, points, 0.0);

    for (int i = 0; i < time_steps; i++)
        time_stamps[i] = i * step_size;

    FILE* out_file = fopen(opts.output_file, "w");
    if (!out_file) {
        fprintf(stderr, "Could not open output file: %s\n", opts.output_file);
        exit(1);
    }

    print_header(out_file, points);

    for (int i = 0; i < time_steps; i++)
    {
        update_positions(positions, points, time_stamps[i]);

        fprintf(out_file, "%d, %lf", i, time_stamps[i]);

        for (int j = 0; j < points; j++)
            fprintf(out_file, ", %lf", positions[j]);

        fprintf(out_file, "\n");
    }

    fclose(out_file);
    free(time_stamps);
    free(positions);

    return 0;
}

ProgramOptions check_args(int argc, char **argv)
{
    ProgramOptions opts;

    if (argc != 5) {
        fprintf(stderr,
            "Usage: %s <points> <cycles> <samples> <output_file>\n",
            argv[0]);
        exit(1);
    }

    opts.points  = atoi(argv[1]);
    opts.cycles  = atoi(argv[2]);
    opts.samples = atoi(argv[3]);
    strcpy(opts.output_file, argv[4]);

    if (opts.points <= 0 || opts.cycles <= 0 || opts.samples <= 0) {
        fprintf(stderr, "All numerical arguments must be positive.\n");
        exit(1);
    }

    return opts;
}

void print_header(FILE* out_file, int points)
{
    fprintf(out_file, "#, time");
    for (int j = 0; j < points; j++)
        fprintf(out_file, ", y[%d]", j);
    fprintf(out_file, "\n");
}

double driver(double time)
{
    return sin(time * 2.0 * M_PI);
}

void update_positions(double* positions, int points, double time)
{
    double previous = positions[0];
    positions[0] = driver(time);

    for (int i = 1; i < points; i++) {
        double temp = positions[i];
        positions[i] = previous;
        previous = temp;
    }
}

void initialise_vector(double vector[], int size, double initial)
{
    for (int i = 0; i < size; i++)
        vector[i] = initial;
}
