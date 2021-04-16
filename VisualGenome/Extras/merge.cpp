// g++ merge.cpp

#include <bits/stdc++.h>

using namespace std;

#define ll long long

int main(){
	vector<ll> imageId;
	ifstream input("graphToImage.db");
	ll gid,iid;
	while(input>>gid){
		input>>iid;
		imageId.push_back(iid);
	}
	input.close();

	map<ll,string> mp;
	input.open("imageToUrl.db");
	string url;
	while(input>>iid){
		input>>url;
		mp[iid] = url;
	}

	ofstream output("../Output/graphToImageMap.txt");
	for(ll i=0;i<imageId.size();i++){
		output<<i+1<<" "<<imageId[i]<<" "<<mp[imageId[i]]<<endl;
	}
	output.close();
	return 0;
}