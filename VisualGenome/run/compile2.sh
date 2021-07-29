#!/bin/bash

read -p 'Enter number of process : ' N 

echo ------------------------------------
echo ------------------------------------

echo Merging results ...
for ((i = 1 ; i <= $N ; i++)); do
	cat "../Output/${i}a.txt" >> ../Output/result-no-edges-all.txt
  	cat "../Output/${i}b.txt" >> ../Output/result-edges-all.txt
done

echo ------------------------------------
echo ------------------------------------

echo Cleaning up redundant files ...
for ((i = 1 ; i <= $N ; i++)); do
  	#rm $i
  	rm "../Output/${i}a.txt"
  	rm "../Output/${i}b.txt"
done
echo Cleaniing complete

echo ------------------------------------
echo ------------------------------------
