// Run using the following command
// g++ sceneGraph.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;

	#define endl "\n"
	
	ifstream inputSceneGraph;
	ofstream outputGraph;
	long long graphId;
	Value objectLabel;
	Value edgeLabel;
	Value attributeLabel;

	void extractSingleSceneGraph(){
		ofstream outputSceneGraph("scene.json");
		outputSceneGraph<<"{";
		int cntCurly = 1;
		char c;
		while(inputSceneGraph>>noskipws>>c){
			outputSceneGraph<<c;
			if(c=='{') cntCurly++;
			if(c=='}'){
				cntCurly--;
				if(cntCurly==0) break;
			}
		}
		outputSceneGraph.close();
		return;
	}

	void parseJsonToGraph(){
		ifstream inputGraph("scene.json");
		Reader reader;
		Value sceneGraph;
		reader.parse(inputGraph,sceneGraph);
		inputGraph.close();

		map<long long,int> mp;
		outputGraph<<"t # "<<graphId<<endl;
		graphId++;

		Value objects = sceneGraph["objects"];
		vector<pair<int,int>> attributeEdges;
		int offset = 0;
		for(int i=0;i<objects.size();i++){
			long long objectId = objects[i]["object_id"].asInt64();
			string synset = objects[i]["synsets"][0].asString();
			mp[objectId] = i+offset;
			outputGraph<<"v "<<i+offset<<" "<<objectLabel[synset].asInt64()<<endl;
			Value attributes = objects[i]["attributes"];
			int base = i+offset;
			for(int j=0;j<attributes.size();j++){
				offset++;
				string attrName = attributes[j].asString();
				outputGraph<<"v "<<i+offset<<" "<<attributeLabel[attrName].asInt64()<<endl;
				attributeEdges.push_back({base,i+offset});
			}
		}

		Value edges = sceneGraph["relationships"];
		for(int i=0;i<edges.size();i++){
			long long node1 = edges[i]["object_id"].asInt64();
			long long node2 = edges[i]["subject_id"].asInt64();
			string synset = edges[i]["synsets"][0].asString();
			outputGraph<<"e "<<mp[node1]<<" "<<mp[node2]<<" "<<edgeLabel[synset].asInt64()<<endl;
		}

		for(int i=0;i<attributeEdges.size();i++){
			outputGraph<<"e "<<attributeEdges[i].first<<" "<<attributeEdges[i].second<<" 0"<<endl;
		}
	}

	int main(){
		ifstream fObjectLabel("../Labels/node_labels.json");
		ifstream fEdgeLabel("../Labels/edge_labels.json");
		ifstream fAttributeLabel("../Labels/attribute_labels_name.json");
		Reader reader;
		reader.parse(fObjectLabel,objectLabel);
		reader.parse(fEdgeLabel,edgeLabel);
		reader.parse(fAttributeLabel,attributeLabel);
		fObjectLabel.close();
		fEdgeLabel.close();
		fAttributeLabel.close();

		inputSceneGraph.open("../Dataset/scene_graphs.json");
		outputGraph.open("input.db");
		graphId = 1;
		char c;
		while(inputSceneGraph>>noskipws>>c){
			if(c!='{') continue;
			extractSingleSceneGraph();
			parseJsonToGraph();
		}
		inputSceneGraph.close();
		outputGraph.close();
		return 0;
	}

