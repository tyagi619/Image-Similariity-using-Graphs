#!/bin/bash

echo Running VEO on mapillary dataset

echo ------------------------------------
echo ------------------------------------

echo Computing pairwise VEO similarity...
g++ ../VEO/veo.cpp
./a.out
rm a.out

echo ------------------------------------
echo ------------------------------------

echo Sorting results ...
g++ ../VEO/sortResults.cpp
./a.out
rm a.out
rm ../Output/result.txt

echo ------------------------------------
echo ------------------------------------

echo Complete