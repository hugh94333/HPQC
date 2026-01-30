#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int N = atoi(argv[1]);

    clock_t start = clock();

    FILE *f = fopen("data/output.txt", "w");
    for (int i = 0; i < N; i++) {
        fprintf(f, "%d\n", i);
    }
    fclose(f);

    clock_t end = clock();
    double runtime = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Write time: %f\n", runtime);
    return 0;
}
