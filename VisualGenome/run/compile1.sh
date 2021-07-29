#!/bin/bash

read -p 'Enter number of process : ' N 

echo ------------------------------------
echo ------------------------------------

echo Merging results ...
for ((i = 1 ; i <= $N ; i++)); do
  	cat "../Output/$i.txt" >> ../Output/result-improved-all.txt
done

echo ------------------------------------
echo ------------------------------------

echo Cleaning up redundant files ...
for ((i = 1 ; i <= $N ; i++)); do
  	#rm $i
  	rm "../Output/$i.txt"
done
echo Cleaniing complete

echo ------------------------------------
echo ------------------------------------
