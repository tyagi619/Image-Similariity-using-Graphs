// Run using the following command
// g++ sortResults.cpp

	#include <bits/stdc++.h>

	using namespace std;

	int main(){
		int g1,g2;
		double sim;
		vector<pair<double,pair<int,int>>> unsrtResult;
		
		ifstream results("../Output/result.txt");
		while(results>>g1){
			results>>g2>>sim;
			unsrtResult.push_back({sim,{g1,g2}});
		}
		results.close();
		
		sort(unsrtResult.begin(),unsrtResult.end());
		reverse(unsrtResult.begin(),unsrtResult.end());
		
		ofstream srtResults("../Output/similarPairs.txt");
		srtResults<<fixed<<setprecision(3);
		for(int i=0;i<unsrtResult.size();i++){
			srtResults<<unsrtResult[i].second.first<<" "<<unsrtResult[i].second.second<<" "<<unsrtResult[i].first<<endl;
		}
		srtResults.close();
		return 0;
	}

