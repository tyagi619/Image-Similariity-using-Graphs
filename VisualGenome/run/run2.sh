#!/bin/bash

read -p 'Enter number of process : ' N
echo Preparing data for each process ...
g++ ../VEO/divideData.cpp
./a.out $N
rm a.out
echo Data preparation complete

echo ------------------------------------
echo ------------------------------------

echo Running processes ...
for ((i = 1 ; i <= $N ; i++)); do
	g++ -o $i ../VEO/improvedVEO.cpp
	./$i $i &
	echo Process $i started
done

echo ------------------------------------
echo ------------------------------------

echo Process have been started
echo Check top -i to monitor the status of processes

echo ------------------------------------
echo ------------------------------------
