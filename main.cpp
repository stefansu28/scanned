#include <stdio.h>
#include <cmath>

#define PI 3.14159265
#define SAMPLE_RATE 44100


// output a sine wave
// file - the raw output file to write the wave to
// note - the note to generate 0 being C4, 1 being D4, etc
// duration - the duration of the note in seconds
// volume - 0 being completely silent, 1 being full volume
void pulse(FILE *file, int note, float duration, float volume) {
    // convert the note to a frequency
    // 0 is C4
    // 440 is A4
    float freq = 440 * exp2((float)(note + 3) /12);

    float *buffer = NULL;
    int numSamples = (int) (duration * SAMPLE_RATE);
    buffer = (float *) malloc(sizeof(float) * numSamples);
    for (int i = 0; i < numSamples; i++) {
        buffer[i] = sin(freq*(2*PI) * i / SAMPLE_RATE) * volume;
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
        pulse(output, notes[n], .25, .5);
    }

    fclose(output);
    return 0;
}
