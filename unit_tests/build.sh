#!/bin/sh
g++ shuffle.cpp ../src/io/playlist.cpp ../src/data/merge_sort.cpp -I ../include/ -o shuffle.o
g++ properties.cpp ../src/io/audio_file.cpp -I ../include/ -lSDL2 -o properties.o
g++ complex.cpp -o complex.o
g++ scale.cpp -o scale.o
g++ phase.cpp -o phase.o