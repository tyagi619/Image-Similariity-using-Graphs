#!/bin/bash

echo Graph generation and other preprocessing ...

echo ------------------------------------
echo ------------------------------------

echo Generating input graph file ...
echo This may take some time. Please wait
g++ ../GenGraph/mapillary.cpp `pkg-config --cflags --libs opencv` -ljsoncpp
./a.out
rm a.out
echo Input file generatiion complete

echo ------------------------------------
echo ------------------------------------

echo Preprocessing and scene graph generation complete