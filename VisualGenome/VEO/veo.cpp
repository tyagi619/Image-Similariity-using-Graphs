    #include <bits/stdc++.h>
	#include "Hungarian.h"

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
   	bool USE_GREEDY = 1;
   	bool PARTIAL_MATCH = 0;
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

	double hungarianCost(vector<int> &a,vector<int> &b,vector<int> &objectToLabel, vector<vector<double>> &pairSimScore){
		int n = a.size();
		int m = b.size();
		double maxCost = 0.0;
		vector<vector<double>> costMatrix(n,vector<double>(m));
		for(int i=0;i<n;i++){
			int label1 = objectToLabel[a[i]];
			for(int j=0;j<m;j++){
				int label2 = objectToLabel[b[j]];
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
				maxCost = max(cost,maxCost);
				costMatrix[i][j] = cost;
			}
		}

		for(int i=0;i<n;i++){
			for(int j=0;j<m;j++){
				costMatrix[i][j] = maxCost - costMatrix[i][j];
			}
		}

		vector<int> assignment;

		HungarianAlgorithm HungAlgo;
		double cost = HungAlgo.Solve(costMatrix, assignment);
		cost = min(m,n)*maxCost - cost;
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

	double simpleVEO(vector<int> &a,vector<int> &b, vector<int> &objectToLabel, vector<vector<double>> &pairSimScore){
		int n,m;
		n = a.size();
		m = b.size();
		int i=0,j=0;
		double commonNodes = 0;
		while(i<n && j<m){
			if(a[i] == b[j]){
				commonNodes++;
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
		return (double)commonNodes;
	}

	double nodeSimilarityScore(vector<int> &a,vector<int> &b, vector<int> &objectToLabel, vector<vector<double>> &pairSimScore){
		if(!PARTIAL_MATCH){
			return simpleVEO(a,b,objectToLabel,pairSimScore);
		}
		else{
			if(USE_GREEDY){
				return greedyScore(a,b,objectToLabel,pairSimScore);
			}
			else{
				return hungarianCost(a,b,objectToLabel,pairSimScore);
			}
		}
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

	void greedyMaximumBipartiteMatch(int start_i,int start_j,int end_i,int end_j,int threadId){
		string filename = "../Output/" + to_string(threadId) + ".txt";
		
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

		vector<pair<double,pair<int,int>>> similarPairs;
		
		bool start = 0,stop = 0;
		for(int i=start_i;i<=end_i;i++){
			for(int j=i+1;j<=NUM_GRAPHS;j++){
				if(i==start_i && j==start_j){
					start = 1;
				}
				if(i==end_i && j==end_j){
					stop = 1;
					start = 0;
				}
				if(stop) break;
				if(start){
					double totalNodesAndEdges = nodes[i].size() + nodes[j].size() + edges[i].size() + edges[j].size();
					double simScore = 0.0;
					if(totalNodesAndEdges > 0){
		    			double nodeSimScore = nodeSimilarityScore(nodes[i],nodes[j],objectToLabel,pairSimScore);
		            	double edgeSimScore = edgeSimilarityScore(edges[i],edges[j]);
		    			simScore = 2*(nodeSimScore + edgeSimScore)/totalNodesAndEdges;
		            }

		            if(simScore >= SIM_SCORE_LIMIT){
		            	similarPairs.push_back({simScore,{i,j}});
		            }
				}
			}
			if(stop) break;
		}

		ofstream fp(filename);
		fp<<fixed<<setprecision(3);
		for(int i=0;i<similarPairs.size();i++){
			fp<<similarPairs[i].S.F<<" "<<similarPairs[i].S.S<<" "<<similarPairs[i].F<<endl;
		}
		fp.close();
	}

    int main(int argc,char *argv[]){
        deathnote();
        int threadId = 0;
        if(argc < 2){
        	cout<<"Insufficient Number of arguments !!!";
        	return 0;
        }

        if(argc >= 2){
        	threadId = stoi(argv[1]);
        }

        if(argc >= 3){
        	PARTIAL_MATCH = stoi(argv[2]);
        }

        if(argc >= 4 && PARTIAL_MATCH == 1){
        	USE_GREEDY = stoi(argv[3]);
        }

        ifstream fp("../data/dataSep.txt");
        int n;
        fp>>n;
        int start_i,start_j,end_i,end_j;
        for(int i=0;i<n;i++){
        	if(i==0){
        		fp>>start_i>>start_j;
        	}
        	else{
        		fp>>end_i>>end_j;
        		if(i==threadId){
        			greedyMaximumBipartiteMatch(start_i,start_j,end_i,end_j,threadId);
        			break;
        		}
        		start_i = end_i;
        		start_j = end_j;
        	}
        }
        fp.close();
        return 0;
    }
    