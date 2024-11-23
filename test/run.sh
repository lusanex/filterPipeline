#!/bin/bash

# Create the bin directory if it doesn't exist
mkdir -p bin

# Compile the code
g++ -Wall main_tests.cpp -o bin/tests

# Check if the compilation succeeded
if [ $? -eq 0 ]; then
    # Run the tests
    ./bin/tests
else
    echo "Compilation failed. Please check your code."
fi
