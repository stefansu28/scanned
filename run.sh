#!/bin/bash

gcc -lm main.cpp && ./a.out && ffplay -showmode 1 -f f32le -ar 44100 output.raw
