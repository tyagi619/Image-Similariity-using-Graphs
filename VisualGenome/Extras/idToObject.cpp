#include <bits/stdc++.h>

using namespace std;

int main(){
	char c;
	ifstream input("../Labels/node_labels.json");
	ofstream output("../Extras/label_objects.txt");
	while(input>>noskipws>>c){
		if(c!='"') continue;
		string word;
		while(input>>noskipws>>c){
			if(c=='"') break;
			if(c=='_') c=' ';
			word += c;
		}
		word = word.substr(0,word.find('.'));
		int labelId;
		input>>skipws>>c>>labelId;
		output<<labelId<<":"<<word<<","<<endl;
	}
	input.close();
	output.close();
	return 0;
}