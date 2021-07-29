#!/bin/bash

echo Graph generation and other preprocessing ...

echo ------------------------------------
echo ------------------------------------

echo Extracting distinct object classes ...
g++ ../GenLabel/labelNode.cpp -ljsoncpp
./a.out
rm a.out
echo Extraction Complete

echo ------------------------------------
echo ------------------------------------

echo Extracting distinct relationship classes ...
g++ ../GenLabel/labelEdge.cpp -ljsoncpp
./a.out
rm a.out
echo Extraction Complete

echo ------------------------------------
echo ------------------------------------

echo Generating input graph file ...
echo This may take some time. Please wait
g++ ../GenGraph/visualGenome.cpp -ljsoncpp
./a.out
rm a.out
rm scene.json
echo Input file generatiion complete

echo ------------------------------------
echo ------------------------------------

echo Mapping graph number to image id and url ....
g++ ../Extras/imageUrl.cpp -ljsoncpp
./a.out
rm a.out
rm image.json

g++ ../Extras/merge.cpp
./a.out
rm a.out
rm graphToImage.db
rm imageToUrl.db
echo Mapping Complete

echo ------------------------------------
echo ------------------------------------

echo Calculating cosine similarity between vertex labels....
echo 'Part (0/4) complete'
g++ ../Extras/idToObject.cpp
./a.out
rm a.out
echo 'Part (1/4) complete'
g++ ../Extras/labelToId.cpp
./a.out
rm a.out
echo 'Part (2/4) complete'
g++ ../Extras/label_objects.cpp
./a.out
rm a.out
echo 'Part (3/4) complete'
g++ ../Extras/label_edges.cpp
./a.out
rm a.out
g++ ../Extras/node_vec.cpp
./a.out
rm a.out
rm ../Extras/*.txt
echo 'Part (4/4) complete'

echo ------------------------------------
echo ------------------------------------

echo Preprocessing and scene graph generation complete
