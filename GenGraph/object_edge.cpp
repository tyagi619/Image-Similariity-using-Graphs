// Run using the following command
// g++ object_edge.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;

	#define endl "\n"
	ifstream inputObject;
	ifstream inputEdge;

	Value objectLabel;
	Value edgeLabel;

	void extractSingleGraphObjects(){
		ofstream outputObject("object1.json");
		outputObject<<"{";
		int cntCurly = 1;
		char c;
		while(inputObject>>noskipws>>c){
			outputObject<<c;
			if(c=='{') cntCurly++;
			if(c=='}'){
				cntCurly--;
				if(cntCurly==0) break;
			}
		}
		outputObject.close();
		return;
	}

	void extractSingleGraphEdges(){
		ofstream outputEdge("relation1.json");
		outputEdge<<"{";
		int cntCurly = 1;
		char c;
		while(inputEdge>>noskipws>>c){
			outputEdge<<c;
			if(c=='{') cntCurly++;
			if(c=='}'){
				cntCurly--;
				if(cntCurly==0) break;
			}
		}
		outputEdge.close();
		return;
	}

	bool parseJsonToGraph(int id){
		ifstream inputObjectGraph("object1.json");
		ifstream inputRelationGraph("relation1.json");
		Reader reader;
		Value ObjectGraph;
		Value RelationGraph;
		reader.parse(inputObjectGraph,ObjectGraph);
		reader.parse(inputRelationGraph,RelationGraph);
		inputObjectGraph.close();
		inputRelationGraph.close();

		if(ObjectGraph["image_id"].asInt() == id){
			return true;
		}
		return false;
	}

	int main(){
		ifstream fObjectLabel("../Labels/node_labels.json");
		ifstream fEdgeLabel("../Labels/edge_labels.json");
		Reader reader;
		reader.parse(fObjectLabel,objectLabel);
		reader.parse(fEdgeLabel,edgeLabel);
		fObjectLabel.close();
		fEdgeLabel.close();
		
		int imageId;
		cout<<"Enter Image Id : ";
		cin>>imageId;
		
		inputObject.open("../Dataset/objects.json");
		inputEdge.open("../Dataset/relationships.json");
		char c;
		while(inputObject>>noskipws>>c){
			if(c!='{') continue;
		    extractSingleGraphObjects();
			
			while(inputEdge>>noskipws>>c){
				if(c=='{') break;
			}
			extractSingleGraphEdges();
			if(parseJsonToGraph(imageId)){
				break;
			}
		}
		inputObject.close();
		inputEdge.close();
		return 0;
	}

