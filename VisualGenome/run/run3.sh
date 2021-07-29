#!/bin/bash

read -p 'Enter number of process : ' N
echo ------------------------------------
echo ------------------------------------

echo Running processes ...
for ((i = 1 ; i <= $N ; i++)); do
	g++ -o $i ../VEO/improvedVEO-all.cpp
	./$i $i &
	echo Process $i started
done

echo ------------------------------------
echo ------------------------------------

echo Process have been started
echo Check top -i to monitor the status of processes

echo ------------------------------------
echo ------------------------------------
