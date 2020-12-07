// Run using the following command
// g++ labelNode.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;

	#define endl "\n"

	int main(){
		ifstream inputJson("../Dataset/object_synsets.json");
		Reader reader;
		Value config;
		reader.parse(inputJson,config);
		
		StyledWriter writer;
		Value lblnode;
		
        cout<<"Total number of objects : "<<config.size()<<endl;

		long long lblId = 1;
		ValueIterator itr;
		for(itr=config.begin() ; itr!=config.end() ; itr++){
            string key = config[itr.name()].asString();
            int check = lblnode.get(key,-1).asInt64();
            if(check==-1)
                lblnode[key] = lblId++;
		}
		
        cout<<"Number of distinct object classes : "<<lblId-1<<endl;

		ofstream outputJson("../Labels/node_labels.json");
		outputJson << writer.write(lblnode);

		return 0;
	}

