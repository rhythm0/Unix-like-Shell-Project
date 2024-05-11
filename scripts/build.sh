#!/bin/bash

# Navigate to the project root directory from scripts directory
cd ..

# Compile the msh program and place the executable in the bin directory
gcc -I./include/ -o ./bin/msh src/*.c

cd scripts
