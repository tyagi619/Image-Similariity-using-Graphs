#!/bin/bash

declare -i useGreedy
useGreedy=0

declare -i usePartial
usePartial=0

read -p 'Enter number of process : ' N
read -p 'Use Partial Matching for vertex (Y/N) : ' pM

if [[ $pM == "Y" || $pM == "y" ]]
then
	usePartial=1
	read -p 'Use greedy approach (Y/N) : ' ch
	if [[ $ch == "y" || $ch == "Y" ]] 
	then
  		useGreedy=1
	fi
fi

echo ------------------------------------
echo ------------------------------------

echo Use Partial Matching - $usePartial
if(( $usePartial == 1 ))
then
	echo Use Greedy Partial Matching - $useGreedy
fi

echo ------------------------------------
echo ------------------------------------

echo Preparing data for each process ...
g++ ../VEO/divideData.cpp
./a.out $N
rm a.out
echo Data preparation complete

echo ------------------------------------
echo ------------------------------------

echo Running processes ...
for ((i = 1 ; i <= $N ; i++)); do
	g++ -o $i ../VEO/Hungarian.cpp ../VEO/veo.cpp
	./$i $i $usePartial $useGreedy &
	echo Process $i started
done

echo ------------------------------------
echo ------------------------------------

echo Process have been started
echo Check top -i to monitor the status of processes

echo ------------------------------------
echo ------------------------------------
