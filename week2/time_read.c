#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    clock_t start = clock();

    FILE *f = fopen("data/output.txt", "r");
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), f)) {
        // read into memory
    }
    fclose(f);

    clock_t end = clock();
    double runtime = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Read time: %f\n", runtime);
    return 0;
}
