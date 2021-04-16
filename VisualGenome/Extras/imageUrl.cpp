// Run using the following command
// g++ imageUrl.cpp -ljsoncpp

	#include <bits/stdc++.h>
	#include <jsoncpp/json/json.h>

	using namespace std;
	using namespace Json;
	
	ifstream inputSceneGraph;
	ofstream imageUrl;

	void extractSingleImage(){
		ofstream imageDetail("image.json");
		imageDetail<<"{";
		int cntCurly = 1;
		char c;
		while(inputSceneGraph>>noskipws>>c){
			imageDetail<<c;
			if(c=='{') cntCurly++;
			if(c=='}'){
				cntCurly--;
				if(cntCurly==0) break;
			}
		}
		imageDetail.close();
		return;
	}

	void getUrl(){
		ifstream imageDetail("image.json");
		Reader reader;
		Value image;
		reader.parse(imageDetail,image);
		imageDetail.close();

		imageUrl<<image["image_id"].asString()<<" "<<image["url"].asString()<<endl;
	}

	int main(){
		inputSceneGraph.open("../Dataset/image_data.json");
		imageUrl.open("imageToUrl.db");
		char c;
		while(inputSceneGraph>>noskipws>>c){
			if(c!='{') continue;
			extractSingleImage();
			getUrl();
		}
		inputSceneGraph.close();
		imageUrl.close();
		return 0;
	}

