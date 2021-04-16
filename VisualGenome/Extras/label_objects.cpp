#include <bits/stdc++.h>

using namespace std;

map<int,string> mpp;
map<string,int> mp;

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
	ifstream input("../Extras/labelToId.txt");
	int id;
	char c;
	while(input>>noskipws>>c){
		if(c=='\n') continue;
		string label = "";
		label += c;
		while(input>>noskipws>>c){
			if(c==',') break;
			label += c; 
		}

		input>>skipws>>id;
		mp[label] = id;
	}
	input.close();
}

int main(){
	loadIndex();
	loadObject();
	map<int,string>::iterator it;
	ofstream fp("../data/labelObject.txt");
	for(it=mpp.begin();it!=mpp.end();it++){
		string label = it->second;
		fp<<it->first<<" "<<mp[label]<<endl;
	}
	fp.close();
	return 0;
}