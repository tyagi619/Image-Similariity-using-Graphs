    #include <bits/stdc++.h>

    using namespace std;
/******************************************************************************************/
    typedef long long ll;
    typedef long double ld;
    typedef vector<ll> vll;
    typedef vector<int> vi;
    typedef pair<ll,ll> pll;
    typedef pair<int,int> ii;
    typedef vector<ii> vii;
/****************************************************************************************************/
    #define deathnote()     ios_base::sync_with_stdio(0); cin.tie(0); cout.tie(0);
    #define F               first
    #define S               second
    #define NUM_GRAPHS  	89946
    #define NUM_LABELS		5774
    #define NUM_OBJECT_CLASS 5996
   	#define SIM_SCORE_LIMIT 0.8 
   	#define NUM_ITERATIONS  5
/****************************************************************************************************/

    void readObjectSimScore(vector<vector<double>> &pairSimScore){
    	int label1,label2;
    	double simScore;
    	ifstream fp("../data/LabelSim.txt");
    	while(fp>>label1){
    		fp>>label2>>simScore;
    		pairSimScore[label1][label2-label1] = simScore;
    	}
    	fp.close();
    }

    void readObjectId(vector<int> &objectToLabel){
    	int id,label;
    	ifstream fp("../data/labelObject.txt");
    	while(fp>>id){
    		fp>>label;
    		objectToLabel[id] = label;
    	}
    	fp.close();
    }

	void readGraphs(vector<vector<int>> &nodes,vector<vector<int>> &edges){
    	char c;
    	int u,v,e,t;
		ifstream fp("../data/input.db");
		while(fp>>c){
	        switch(c){
	            case 't': fp>>c>>t;
	                      if(t>1){
	                          sort(edges[t-1].begin(),edges[t-1].end());
	                          sort(nodes[t-1].begin(),nodes[t-1].end());
	                      }
	                      break;
	            case 'v': fp>>u>>v;
	                      nodes[t].push_back(v);
	                      break;
	            case 'e': fp>>u>>v>>e;
	                      edges[t].push_back(e);
	                      break;
	        }
	    }
	    fp.close();
	    sort(nodes[NUM_GRAPHS].begin(),nodes[NUM_GRAPHS].end());
	    sort(edges[NUM_GRAPHS].begin(),edges[NUM_GRAPHS].end());
	}

    int getNum(vector<int> &v){
	    int n = v.size();
	    srand(time(NULL));
	    int index = rand() % n;
	    int num = v[index];
	    swap(v[index], v[n - 1]);
	    v.pop_back();
	    return num;
	}
 
	vector<int> generateRandom(int n){
	    vector<int> v(n);
	    for (int i = 0; i < n; i++)
	        v[i] = i + 1;
	    vector<int> ans;
	    while (v.size()) {
	        ans.push_back(getNum(v));
	    }
	    return ans;
	}

	double greedyMatchScore(vector<vector<double>> &costMatrix){
		int n = costMatrix.size();
		if(n < 1) return 0.0;
		int m = costMatrix[0].size();
		vector<int> randomOrder = generateRandom(n);
		vector<bool> vis(m,0);

		double cost = 0.0;
		for(int i=0;i<n;i++){
			int index = randomOrder[i]-1;
			double maxCost = -1;
			int maxIndex = -1;
			for(int j=0;j<m;j++){
				if(!vis[j] && costMatrix[index][j] > maxCost){
					maxCost = costMatrix[index][j];
					maxIndex = j;
				}
			}
			if(maxIndex != -1){
				vis[maxIndex] = 1;
				cost += maxCost;
			}
		}
		return cost;
	}

	double greedyScore(vector<int> &a,vector<int> &b, vector<int> &objectToLabel, vector<vector<double>> &pairSimScore){
		int n,m;
		n = a.size();
		m = b.size();
		int i=0,j=0;
		double commonNodes = 0;
		vector<int> distictNodesA,distinctNodesB;
		while(i<n && j<m){
			if(a[i] == b[j]){
				commonNodes++;
				i++;
				j++;
			}
			else if(a[i] < b[j]){
				distictNodesA.push_back(a[i]);
				i++;
			}
			else{
				distinctNodesB.push_back(b[j]);
				j++;
			}
		}

		while(i<n){
			distictNodesA.push_back(a[i]);
			i++;
		}
		while(j<m){
			distinctNodesB.push_back(b[j]);
			j++;
		}

		n = distictNodesA.size();
		m = distinctNodesB.size();
		vector<vector<double>> costMatrix(n,vector<double>(m));
		for(int i=0;i<n;i++){
			int label1 = objectToLabel[distictNodesA[i]];
			for(int j=0;j<m;j++){
				int label2 = objectToLabel[distinctNodesB[j]];
				double cost;
				if(label1 == label2){
					cost = 1.0;
				}
				else if(label1 < label2){
					cost = pairSimScore[label1][label2-label1];
				}
				else if(label2 < label1){
					cost = pairSimScore[label2][label1-label2];
				}
				else{
					cost = 0.0;
				}
				if(cost < 0.0) cost = 0.0;
				costMatrix[i][j] = cost;
			}
		}

		double maxCost = 0.0;
		for(int i=0;i<NUM_ITERATIONS;i++){
			maxCost = max(maxCost,greedyMatchScore(costMatrix));
		}
		return (commonNodes+maxCost);
	}

	double nodeSimilarityScore(vector<int> &a,vector<int> &b, vector<int> &objectToLabel, vector<vector<double>> &pairSimScore){
		return greedyScore(a,b,objectToLabel,pairSimScore);
	}

	double edgeSimilarityScore(vector<int> &a, vector<int> &b){
		int n,m;
		n = a.size();
		m = b.size();
		int i=0,j=0;
		int commonEdges = 0;
		while(i<n && j<m){
			if(a[i] == b[j]){
				commonEdges++;
				i++;
				j++;
			}
			else if(a[i] < b[j]){
				i++;
			}
			else{
				j++;
			}
		}
		return (double)commonEdges;
	}

	void greedyMaximumBipartiteMatch(int start_i,int end_i,int threadId){
		string filename1 = "../Output/" + to_string(threadId) + "a.txt";
		string filename2 = "../Output/" + to_string(threadId) + "b.txt";
		
		vector<vector<int>> nodes(NUM_GRAPHS + 1);
    	vector<vector<int>> edges(NUM_GRAPHS + 1); 
		readGraphs(nodes,edges);

		vector<int> objectToLabel(NUM_OBJECT_CLASS + 1);
		readObjectId(objectToLabel);

		vector<vector<double>> pairSimScore(NUM_LABELS + 1);
		for(int i=1;i<=NUM_LABELS;i++){
			for(int j=0;j<NUM_LABELS-i+1;j++){
				pairSimScore[i].push_back(0.0);
			}
		}
		readObjectSimScore(pairSimScore);

		vector<vector<pair<double,int>>> similarPairs1(NUM_GRAPHS+1);
		vector<vector<pair<double,int>>> similarPairs2(NUM_GRAPHS+1);
		
		for(int i=start_i;i<=end_i;i++){
			vector<pair<double,int>> scores1,scores2;
			for(int j=1;j<=NUM_GRAPHS;j++){
				if(j==i) continue;
				double totalNodesAndEdges = nodes[i].size() + nodes[j].size() + edges[i].size() + edges[j].size();
				double simScore1 = 0.0, simScore2=0.0;
				if(totalNodesAndEdges > 0){
		    		double nodeSimScore = nodeSimilarityScore(nodes[i],nodes[j],objectToLabel,pairSimScore);
		            double edgeSimScore = edgeSimilarityScore(edges[i],edges[j]);
		    		simScore1 = 2*(nodeSimScore + edgeSimScore)/totalNodesAndEdges;
		    		simScore2 = 2*(nodeSimScore)/(nodes[i].size() + nodes[j].size());
		        }
		        scores1.push_back({simScore1,j});
		        scores2.push_back({simScore2,j});
			}
			sort(scores1.begin(),scores1.end());
			int len_scores = scores1.size();
			for(int j=0;j<10;j++){
				similarPairs1[i].push_back(scores1[len_scores-j-1]);
			}
			scores1.clear();

			sort(scores2.begin(),scores2.end());
			len_scores = scores2.size();
			for(int j=0;j<10;j++){
				similarPairs2[i].push_back(scores2[len_scores-j-1]);
			}
			scores2.clear();
		}

		ofstream fp(filename1);
		fp<<fixed<<setprecision(3);
		for(int i=start_i;i<=end_i;i++){
			fp<<i<<" ";
			for(int j=0;j<similarPairs1[i].size();j++){
				fp<<similarPairs1[i][j].S<<" "<<similarPairs1[i][j].F<<" ";
			}
			fp<<endl;
		}
		fp.close();

		fp.open(filename2);
		fp<<fixed<<setprecision(3);
		for(int i=start_i;i<=end_i;i++){
			fp<<i<<" ";
			for(int j=0;j<similarPairs2[i].size();j++){
				fp<<similarPairs2[i][j].S<<" "<<similarPairs2[i][j].F<<" ";
			}
			fp<<endl;
		}
		fp.close();
	}

    int main(int argc,char *argv[]){
        deathnote();
        int threadId = 0;
        if(argc < 2){
        	cout<<"Insufficient Number of arguments !!!\n";
        	return 0;
        }

        if(argc >= 2){
        	threadId = stoi(argv[1]);
        }

        int start_i,end_i;
        int i = threadId;
        int bins = ceil((double)NUM_GRAPHS/24);
        start_i = (i-1)*bins + 1;
        end_i = min(i*bins,NUM_GRAPHS);
        greedyMaximumBipartiteMatch(start_i,end_i,threadId);
        return 0;
    }
    