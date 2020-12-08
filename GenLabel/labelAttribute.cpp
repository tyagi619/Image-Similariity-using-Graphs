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
		inputJson.close();
		
        cout<<"Total number of attributes : "<<config.size()<<endl;

		Value lblattribute;
		long long lblId = 1;
		long long offset = 6000;
		ValueIterator itr;
		for(itr=config.begin() ; itr!=config.end() ; itr++){
            string key = config[itr.name()].asString();
            int check = lblattribute.get(key,-1).asInt64();
            if(check==-1){
                lblattribute[key] = offset + (lblId);
				lblId++;
			}
		}
		
        cout<<"Number of distinct attribute classes : "<<lblId-1<<endl;

		Value lblNameAttribute;
		for(itr=config.begin() ; itr!=config.end() ; itr++){
            string key = config[itr.name()].asString();
			lblNameAttribute[itr.key().asString()] = lblattribute[key].asInt64();
		}

		StyledWriter writer;
		ofstream outputJson("../Labels/attribute_labels.json");
		outputJson << writer.write(lblattribute);
		outputJson.close();
		outputJson.open("../Labels/attribute_labels_name.json");
		outputJson << writer.write(lblNameAttribute);
		outputJson.close();
		return 0;
	}

