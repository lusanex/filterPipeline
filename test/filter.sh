#!/bin/bash

mkdir -p bin 
mkdir -p out

g++ -g TestFilterPipeLine.h pipeline_main.cpp -o bin/filter && ./bin/filter && (eog out/final_filter.bmp &) && (eog assets/lena_color.bmp)
