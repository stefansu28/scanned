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

// Description of the system
// contains hookes laws constants and dampening
// TODO do constants per system as well as matrix of connection between points
struct SystemDesc {
    float k, b;
};

// state of the scanned synthesis system
// contains the position and velocity of each point mass
struct ScanState {
    int size; // the number of points
    float *position;
    float *velocity;
};

// scan through the state
// file - the raw output file to write the wave to
// state - the initial state of the system, this will be modified!!!
// system - contains system constants
// scanFreq - how fast to scan through the state, this is the perceived pitch
// updateRate - this is the step size (in seconds) of the dynamic system, ie how fast the sound changes timbre and dies out
// duration - the duration of the note in seconds
// TODO volume - 0 being completely silent, 1 being full volume. Essentially scales the system
void scan(FILE *output, ScanState *state, SystemDesc *system, float scanFreq, float updateRate, float duration) {
    int numSamples = (int) (duration * SAMPLE_RATE);
    float *buffer = NULL;
    buffer = (float *) malloc(sizeof(float) * numSamples);

    for (int i = 0; i < numSamples; i++) {
        int index = (int) (i* scanFreq / (state->size) / SAMPLE_RATE) % state->size;
        buffer[i] = state->position[index];

        // update the system
        for (int j = 0; j < state->size; j++) {
            // get neighbor positions
            // TODO should probably make a copy of the original state and use that to be more accurate
            // TODO should allow the ability to take into account all other points
            float prev = state->position[j > 0 ? j - 1 : state->size - 1];
            float next = state->position[j < state->size - 1 ? j + 1 : 0];

            // Use hookes law with dampening:  F = -kx + bv
            // Distance from equalibrium
            // TODO maybe make this weighted sum
            float deltaX = (prev + next) / 2 - state->position[j];

            // TODO take "k" as parameter of the system, one for each point mass
            // TODO take dampening "b" as parameter of system, one for each point mass
            // TODO maybe also take mass but might not matter, for now just take force to be the acceleration (m = 1)
            float force = system->k * deltaX - system->b * state->velocity[j];


            // TODO make updates use a better form of integration
            state->velocity[j] += force * updateRate;
            state->position[j] += state->velocity[j] * updateRate;
        }
    }

    fwrite(buffer, sizeof(float), numSamples, output);
}

int main(int argc, const char **argv) {
    // the output file, this is where we will dump the raw audio
    FILE *output = NULL;

    if((output = fopen("output.raw", "wb")) == NULL) {
        printf("Failed to open the output file!\n");
        return 1;
    }

    // int notes[] = {
    //     0, 2, 4, 5, 7, 9, 11, 12
    // };

    // for (int n = 0; n < sizeof(notes)/sizeof(int); n++) {
    //     pulse(output, notes[n], .25, .5);
    // }

    SystemDesc system = {1, .01};

    ScanState state;
    state.size = 2^8;
    state.position = (float*) malloc(sizeof(float) * state.size);
    state.velocity = (float*) malloc(sizeof(float) * state.size);

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

    for (int times = 0; times < 5; times++) {
        for (int n = 0; n < sizeof(notes)/sizeof(int); n++) {
            auto note = notes[n] + 12;
            system.k = 1 + 1*(n%2);
            // initialize the state
            for (int i = 0; i < state.size; i++) {
                // just going to make it a giant sine wave for now
                state.position[i] = sin(2*PI*i/((float) (state.size))) * 1;
                // initialize to it being still
                state.velocity[i] = 0;
            }
            // if (times == 0 && n == 0) {
            //     auto index = 0;
            //     int samples = 20;
            //     for (int m = 0; m < samples; m++) {
            //         printf("%f ", state.position[m*state.size/samples]);
            //     }
            //     printf("\n");
            // }

            float freq = 440 * exp2((float)(note + 3) /12);
            // do the thing!!!
            scan(output, &state, &system, freq, .005, 2);
        }
    }

    free(state.position);
    free(state.velocity);

    fclose(output);
    return 0;
}
