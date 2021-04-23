## **Image Similarity using Scene Graphs**

## Introduction
Image similarity is an actively studied topic in recent years because of the
large number of practical applications that it has in the field of social media
analysis, web-image retrieval and digital forensics. The problem of image
similarity can be stated as follows: Given a database or collection of
images and a query image, the task is to retrieve all the images from the
database that are semantically similar to the query image. The task of
image similarity can be visualised as a two-step process : firstly describing
the image contents and then using efficient similarity functions. Most
state-of-art approaches in this field rely on deep neural networks. However,
we propose to use graph similarity metrics on scene graphs as a solution to
this problem.

## Install the following in order to run the code:
   * JsonCpp - for parsing json files. <br/>
     ```sudo apt-get install libjsoncpp-dev```<br/>
   * OpenCV for c++ - for parsing images from mapillary vistas dataset to scene graphs<br/>
     Follow the instructions from http://www.codebind.com/cpp-tutorial/install-opencv-ubuntu-cpp/<br/>


## Steps to run the code

# Mapillary Vistas Dataset
1- Navigate to Mapillary directory <br/>
   ```cd Mapillary```<br/>
2- Download the Mapillary Vistas Dataset v1.2 from https://www.mapillary.com/dataset/vistas?.<br/>Extract the dataset and copy the training folder to the Dataset sub-folder in Mapillary folder.<br/>
3- Navigate to the run folder.<br/>
   ```cd run```<br/>
4- Make the scripts executable<br/>
   ```chmod 777 *.sh```<br/>
5- Generate the scene graph for all the images.<br/>
   ```./genGraph.sh```<br/>
6- Run the Graph Similarity join with edit distance constraints (GSim)<br/>
   ```./runGSim.sh```<br/>
   The script will ask for the size of q-gram and tau as input.<br/>
7- Run the VEO on Mapillary Vistas Dataset<br/>
   ```./runVeo.sh```<br/>
8- To view the results of above algorithms, navigate to the Output folder.<br/>
9- The 'GSim.txt' file contains the output for GSim algorithm<br/>
10-The 'similarPairs.txt' contains the similar pair list obtained using VEO algorithm sorted by similarity score<br/>
11-Navigate to data folder. 'graphToImage.txt' contains the graph number and the corresponding image name<br/>


# Visual Genome Dataset
1- Navigate to VisualGenome directory <br/>
   ```cd VisualGenome```<br/>
2- Download the following files from the dataset: <br/>
	* image_data.json - https://visualgenome.org/static/data/dataset/image_data.json.zip<br/>
	* object_synsets.json - https://visualgenome.org/static/data/dataset/object_synsets.json.zip<br/>
	* relationship_synsets.json - https://visualgenome.org/static/data/dataset/relationship_synsets.json.zip<br/>
	* scene_graphs.json - https://visualgenome.org/static/data/dataset/scene_graphs.json.zip<br/>
   Extract the downloaded files and copy them to the Dataset folder.<br/>
3- Navigate to the run folder.<br/>
   ```cd run```<br/>
4- Make the scripts executable<br/>
   ```chmod 777 *.sh```<br/>
5- Generate the scene graph for all the images and perform other preprocessing steps.<br/>
   ```./genGraph.sh```<br/>
6- Run the VEO algorithm<br/>
   ```./run.sh```<br/>
   The script will prompt the number of parallel processes to run as input.<br/>
   The script will ask if you need to run simple VEO or VEO with partial matching.<br/>
   In case of VEO with partial matching, the script furthur ask to choose between greedy and hungarian algorithm<br/>
7- Compile the results of VEO algorithm<br/>
   ```./compile.sh```<br/>
   The script will prompt the number of parallel processes that had been run as input<br/>
8- To view the results of above algorithms, navigate to the Output folder.<br/>
9-The 'similarPairs.txt' contains the similar pair list obtained using VEO algorithm sorted by similarity score<br/>
10-'graphToImage.txt' contains the graph number and the corresponding image number and the link to image<br/>
