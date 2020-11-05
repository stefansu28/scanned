#include <stdio.h>
#include <cmath>

#include <stdlib.h>
#include <time.h>

#include "../include/scanned.h"

#define PI 3.14159265f
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

    // seed the random generator
    srand(time(NULL));

    // C major scale
    // int notes[] = {
    //     0, 2, 4, 5, 7, 9, 11, 12
    // };

    // C minor
    // int notes[] = {
    //     0, 2, 3, 5, 7, 9, 10, 12
    // };

    int notes[] = {
        0, 0, 5, 5, 1, 1, 0, 0
    };

    // for (int n = 0; n < sizeof(notes)/sizeof(int); n++) {
    //     pulse(output, notes[n], 2, .5);
    // }

    // return 0;

    SystemDesc system = {.8, .01, .016, .01};

    ScanState state;
    state.size = 1<<7;
    // printf("size: %d\n", state.size);
    state.position = (float*) malloc(sizeof(float) * state.size);
    state.velocity = (float*) malloc(sizeof(float) * state.size);

    const float randOffsetRang = 0.f;
    const float randVelRang = 0.f;

    for (int times = 0; times < 5; times++) {
        for (unsigned int n = 0; n < sizeof(notes)/sizeof(int); n++) {
            auto note = notes[n];
            system.k = .8 + .2*((n + 1)%2);
            // initialize the state
            for (int i = 0; i < state.size; i++) {
                // add a little randomness
                float randOffset = rand()/((float)RAND_MAX) * randOffsetRang - (randOffsetRang/2.f);
                // just going to make it a giant sine wave for now
                float amplitude = .1 + .5 * ((n+1) %2);
                state.position[i] = sin(2*PI*i/state.size/4) * amplitude + randOffset;
                // initialize to it being still
                state.velocity[i] = rand()/((float)RAND_MAX) * randVelRang - (randVelRang/2.f);
            }

            float freq = 440 * exp2((float)(note + 3) /12);
            // do the thing!!!
            scan(output, &state, &system, freq, 1);
        }
    }

    free(state.position);
    free(state.velocity);

    fclose(output);
    return 0;
}
