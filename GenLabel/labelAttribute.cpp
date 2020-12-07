// Run using the following command
// g++ labelAttribute.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;

	#define endl "\n"

	int main(){
		ifstream inputJson("../Dataset/attribute_synsets.json");
		Reader reader;
		Value config;
		reader.parse(inputJson,config);
		
		StyledWriter writer;
		Value lblattribute;
		
        cout<<"Total number of attributes : "<<config.size()<<endl;

		long long lblId = 1;
		ValueIterator itr;
		for(itr=config.begin() ; itr!=config.end() ; itr++){
            string key = config[itr.name()].asString();
            int check = lblattribute.get(key,-1).asInt64();
            if(check==-1)
                lblattribute[key] = lblId++;
		}
		
        cout<<"Number of distinct attribute classes : "<<lblId-1<<endl;

		ofstream outputJson("../Labels/attribute_labels.json");
		outputJson << writer.write(lblattribute);

		return 0;
	}

