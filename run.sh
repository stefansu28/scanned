#!/bin/bash

make && ./build/scanned && ffplay -showmode 1 -f f32le -ar 44100 output.raw
