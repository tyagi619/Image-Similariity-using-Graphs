// Run using the following command
// g++ sortResults.cpp

	#include <bits/stdc++.h>

	using namespace std;

	#define endl "\n"
	
	
	int main(){
		ifstream results("simIndex_gte_60.txt");
		int g1,g2;
		double sim;
		vector<pair<double,pair<int,int>>> unsrtResult;
		char c;
		cout<<"Load Starting\n";
		while(results>>g1){
			results>>c>>g2>>c>>c>>c>>sim;
			unsrtResult.push_back({sim,{g1,g2}});
		}
		results.close();
		cout<<"Load Complete\n";
		sort(unsrtResult.begin(),unsrtResult.end());
		reverse(unsrtResult.begin(),unsrtResult.end());
		cout<<"Printing Results\n";
		ofstream srtResults("sortedResults.txt");
		srtResults<<fixed<<setprecision(3);
		for(int i=0;i<unsrtResult.size();i++){
			srtResults<<unsrtResult[i].second.first<<" : "<<unsrtResult[i].second.second<<" --- "<<unsrtResult[i].first<<endl;
		}
		srtResults.close();
		cout<<"COmplete\n";
		return 0;
	}

