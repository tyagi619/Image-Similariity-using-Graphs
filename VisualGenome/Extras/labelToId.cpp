#include <bits/stdc++.h>

using namespace std;

#define D 300

ifstream input;

long double cosineSimilarity(vector<long double> &a, vector<long double> &b){
	long double len_a = 0.0, len_b = 0.0;
	long double dotProd = 0.0;
	for(int i=0;i<D;i++){
		dotProd += a[i]*b[i];
		len_a += a[i]*a[i];
		len_b += b[i]*b[i];
	}
	len_a = sqrt(len_a);
	len_b = sqrt(len_b);
	return dotProd/(len_a*len_b);
}

int main(){
	input.open("../data/wordToVec.txt");
	char c;
	vector<string> wordList;
	vector<vector<long double>> wordVec;
	while(input>>noskipws>>c){
		if(c=='\n' || c==' ') continue;
		string word = "";
		while(c!=':'){
			word+=c;
			input>>c;
		}
		wordList.push_back(word);
		input>>skipws;
		vector<long double> vec(D);
		for(int i=0;i<D;i++){
			input>>vec[i];
		}
		wordVec.push_back(vec);
	}
	input.close();

	int n = wordList.size();
	ofstream fp("../Extras/labelToId.txt");
	for(int i=0;i<n;i++){
		fp<<wordList[i]<<","<<i+1<<endl;
	}
	fp.close();

	fp.open("../data/LabelSim.txt");
	for(int i=0;i<n;i++){
		for(int j=i+1;j<n;j++){
			long double sim = cosineSimilarity(wordVec[i],wordVec[j]);
			fp<<i+1<<" "<<j+1<<" "<<sim<<endl;
		}
	}
	fp.close();
	return 0;
}