#include<bits/stdc++.h>

using namespace std;

long long NUM_IMAGES = 89946;

int main(int argc,char *argv[]){
	if(argc < 2){
		cout<<"Please specify the number of process as the argument";
		return 0;
	}
	int numProcess = stoi(argv[1]);
	long long count = 0;
	long long grp = 1;
	long long limit = (NUM_IMAGES*(NUM_IMAGES-1))/(2*numProcess);
	ofstream outputFile("../data/dataSep.txt");
	outputFile<<numProcess+1<<endl;
	for(int i=1;i<=NUM_IMAGES;i++){
		for(int j=i+1;j<=NUM_IMAGES;j++){
			count++;
			if(count>limit && grp != numProcess){
				outputFile<<i<<" "<<j<<endl;
				count = 1;
				grp++;
			}

			if(count == 1 && grp == 1){
				outputFile<<i<<" "<<j<<endl;
			}
		}
	}
	outputFile<<NUM_IMAGES<<" "<<NUM_IMAGES<<endl;
	outputFile.close();
}