#include <stdio.h>
#include <cmath>

#define PI 3.14159265
#define SAMPLE_RATE 44100

void pulse(FILE *file, int note, float duration) {
    // convert the note to a frequency
    // 0 is C4
    // 440 is A4
    float freq = 440 * exp2((float)(note + 3) /12);

    float *buffer = NULL;
    int numSamples = (int) (duration * SAMPLE_RATE);
    buffer = (float *) malloc(sizeof(float) * numSamples);
    for (int i = 0; i < numSamples; i++) {
        buffer[i] = sin(freq*(2*PI) * i / SAMPLE_RATE);
        // printf("%f\n", buffer[i]);
    }

    fwrite(buffer, sizeof(float), numSamples, file);

    free(buffer);
}

int main(int argc, const char **argv) {
    // the output file, this is where we will dump the raw audio
    FILE *output = NULL;

    if((output = fopen("output.raw", "wb")) == NULL) {
        printf("Failed to open the output file!\n");
        return 1;
    }

    int notes[] = {
        0, 2, 4, 5, 7, 9, 11, 12
    };

    for (int n = 0; n < sizeof(notes)/sizeof(int); n++) {
        pulse(output, notes[n], .5);
    }

    fclose(output);
    return 0;
}
