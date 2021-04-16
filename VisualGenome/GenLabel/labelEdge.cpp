// Run using the following command
// g++ labelEdge.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;

	int main(){
		ifstream inputJson("../Dataset/relationship_synsets.json");
		Reader reader;
		Value config;
		reader.parse(inputJson,config);
		
		StyledWriter writer;
		Value lbledge;
		
        cout<<"Total number of relations : "<<config.size()<<endl;

		long long lblId = 1;
		ValueIterator itr;
		for(itr=config.begin() ; itr!=config.end() ; itr++){
            string key = config[itr.name()].asString();
            int check = lbledge.get(key,-1).asInt64();
            if(check==-1)
                lbledge[key] = lblId++;
		}
		
        cout<<"Number of distinct relation classes : "<<lblId-1<<endl;

		ofstream outputJson("../Labels/edge_labels.json");
		outputJson << writer.write(lbledge);
		outputJson.close();
		return 0;
	}

