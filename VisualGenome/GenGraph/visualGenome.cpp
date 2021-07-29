// Run using the following command
// g++ sceneGraph.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;
	
	ifstream inputSceneGraph;
	ofstream outputGraph;
	ofstream graphToImage;
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

		Value objects = sceneGraph["objects"];
		int numDistinctVertex = 0;
		map<int,int> mpp;
		for(int i=0;i<objects.size();i++){
			long long objectId = objects[i]["object_id"].asInt64();
			if(objects[i]["synsets"].size()==0){
				continue;
			}
			string synset = objects[i]["synsets"][0].asString();
			if(objectLabel[synset].asInt64()==0){
				continue;
			}

			int objClass = objectLabel[synset].asInt64();
			if(mpp[objClass]==0){
				mpp[objClass] = 1;
				numDistinctVertex++;
			}
		}

		if(numDistinctVertex < 5){
			return;
		}

		outputGraph<<"t # "<<graphId<<endl;
		map<long long,int> mp;

		int offset = 0;
		for(int i=0;i<objects.size();i++){
			long long objectId = objects[i]["object_id"].asInt64();
			if(objects[i]["synsets"].size()==0){
				offset += 1;
				continue;
			}
			string synset = objects[i]["synsets"][0].asString();
			if(objectLabel[synset].asInt64()==0){
				offset += 1;
				continue;
			}
			mp[objectId] = i-offset+1;
			outputGraph<<"v "<<i-offset<<" "<<objectLabel[synset].asInt64()<<endl;
		}

		Value edges = sceneGraph["relationships"];
		for(int i=0;i<edges.size();i++){
			long long node1 = edges[i]["object_id"].asInt64();
			long long node2 = edges[i]["subject_id"].asInt64();
			if(edges[i]["synsets"].size()==0 || mp[node1]==0 || mp[node2]==0){
				continue;
			}
			string synset = edges[i]["synsets"][0].asString();
			if(edgeLabel[synset].asInt64() == 0){
				continue;
			}
			outputGraph<<"e "<<mp[node1]-1<<" "<<mp[node2]-1<<" "<<edgeLabel[synset].asInt64()<<endl;
		}

		graphToImage<<graphId<<" "<<sceneGraph["image_id"]<<endl;
		graphId++;
	}

	int main(){
		ifstream fObjectLabel("../Labels/node_labels.json");
		ifstream fEdgeLabel("../Labels/edge_labels.json");
		Reader reader;
		reader.parse(fObjectLabel,objectLabel);
		reader.parse(fEdgeLabel,edgeLabel);
		fObjectLabel.close();
		fEdgeLabel.close();

		inputSceneGraph.open("../Dataset/scene_graphs.json");
		outputGraph.open("../data/input.db");
		graphToImage.open("graphToImage.db");
		graphId = 1;
		char c;
		while(inputSceneGraph>>noskipws>>c){
			if(c!='{') continue;
			extractSingleSceneGraph();
			parseJsonToGraph();
		}
		inputSceneGraph.close();
		outputGraph.close();
		graphToImage.close();
		return 0;
	}

