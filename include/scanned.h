#ifndef SCANNED_H
#define SCANNED_H

#include <stdio.h>

// Description of the system
// contains hookes laws constants and dampening
// TODO do constants per system as well as matrix of connection between points
typedef struct {
    float k, b;
    // updateRate - this is how much to update the system each iteration (in seconds) of the dynamic system, ie how fast the sound changes timbre and dies out
    // updateSize - this is how small each step is (in seconds)
    float updateRate, updateSize;
} SystemDesc;

// state of the scanned synthesis system
// contains the position and velocity of each point mass
typedef struct {
    int size; // the number of points
    float *position;
    float *velocity;
} ScanState;

// scan through the state
// file - the raw output file to write the wave to
// state - the initial state of the system, this will be modified!!!
// system - contains system constants
// scanFreq - how fast to scan through the state, this is the perceived pitch
// duration - the duration of the note in seconds
void scan(FILE *output, ScanState *state, SystemDesc *system, float scanFreq, float duration);

#endif
