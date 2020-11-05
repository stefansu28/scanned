#include "../include/scanned.h"

#include <stdlib.h>

#define PI 3.14159265f
#define SAMPLE_RATE 44100

// scan through the state
// file - the raw output file to write the wave to
// state - the initial state of the system, this will be modified!!!
// system - contains system constants
// scanFreq - how fast to scan through the state, this is the perceived pitch
// duration - the duration of the note in seconds
void scan(FILE *output, ScanState *state, SystemDesc *system, float scanFreq, float duration) {
    int numSamples = (int) (duration * SAMPLE_RATE);
    float *buffer = NULL;
    buffer = (float *) malloc(sizeof(float) * numSamples);

    // this is for when we update, should use the positions of the point masses from before the update
    // so thats what we store in here
    float *previousPosition = (float *) malloc(sizeof(float) * state->size);

    for (int i = 0; i < numSamples; i++) {
        // scan the system
        // period * frequency * time
        auto scanPos = state->size * scanFreq * ((float) i / SAMPLE_RATE);
        int index = ((int) scanPos) % state->size;
        buffer[i] = state->position[index];

        
        // update the system
        // TODO move to integration function
        float elapsed = 0;
        while(elapsed < system->updateRate) {
            previousPosition[state->size - 1] = state->position[state->size - 1];
            for (int j = 0; j < state->size; j++) {
                auto prevIndex = j > 0 ? j - 1 : state->size - 1;
                auto nextIndex = j < state->size - 1 ? j + 1 : 0;
                previousPosition[j] = state->position[j];
                previousPosition[nextIndex] = state->position[nextIndex];
                // get neighbor positions
                // TODO should allow the ability to take into account all other points
                float prev = previousPosition[prevIndex];
                float next = previousPosition[nextIndex];

                // Use hookes law with dampening:  F = -kx + bv
                // Distance from equalibrium
                // TODO maybe make this weighted sum, add 0 for centering force note "/ 4"
                float deltaX = (prev + next + 0) / 3 - state->position[j];

                // TODO take "k" as parameter of the system, one for each point mass
                // TODO take dampening "b" as parameter of system, one for each point mass
                // TODO maybe also take mass but might not matter, for now just take force to be the acceleration (m = 1)
                float force = system->k * deltaX - system->b * state->velocity[j];

                // TODO make updates use a better form of integration
                state->velocity[j] += force * system->updateSize;
                state->position[j] += state->velocity[j] * system->updateSize;
            }
            elapsed += system->updateSize;
        }
    }

    fwrite(buffer, sizeof(float), numSamples, output);

    free(previousPosition);
}
