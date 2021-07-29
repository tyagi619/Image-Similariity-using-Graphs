#include <bits/stdc++.h>

using namespace std;

#define D 300

map<int,string> mpp;
map<string,vector<double>> mp;

void loadIndex(){
	ifstream input("../Extras/label_objects.txt");
	int id;
	char c;
	while(input>>skipws>>id){
		string label = "";
		while(input>>noskipws>>c){
			if(c==':') break; 
		}

		while(input>>noskipws>>c){
			if(c==',') break;
			label += c;
		}
		mpp[id] = label;
	}
	input.close();
}

void loadObject(){
	ifstream input("../data/wordToVec.txt");
	char c;
	int val;
	while(input>>noskipws>>c){
		if(c=='\n' || c==' ') continue;
		string word = "";
		while(c!=':'){
			word+=c;
			input>>c;
		}

		input>>skipws;
		vector<double> vec(D);
		for(int i=0;i<D;i++){
			input>>vec[i];
		}
		mp[word] = vec;
	}
	input.close();
}

int main(){
	loadIndex();
	loadObject();
	map<int,string>::iterator it;
	ofstream fp("../data/vecNode.txt");
	for(it=mpp.begin();it!=mpp.end();it++){
		string label = it->second;
		fp<<it->first;
		if(mp[label].size()!=300){
			cout<<"Error\n";
		}
		for(int i=0;i<mp[label].size();i++){
			fp<<" "<<mp[label][i];
		}
		fp<<endl;
	}
	fp.close();
	return 0;
}