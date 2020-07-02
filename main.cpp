#include <stdio.h>
#include <cmath>

#define PI 3.14159265

int main(int argc, const char **argv) {
    const int numSteps = 100;

    // the output file, this is where we will dump the raw audio
    FILE *output = NULL;

    if((output = fopen("output.raw", "wb")) == NULL) {
        printf("Failed to open the output file!\n");
        return 1;
    }

    float *buffer = NULL;
    buffer = (float *) malloc(sizeof(float) * numSteps);
    for (int i = 0; i < numSteps; i++) {
        buffer[i] = sin(2*PI * i/numSteps);
    }

    fwrite(buffer, sizeof(float), numSteps, output);

    fclose(output);
    return 0;
}
