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
    #define NUM_EDGE_CLASS	1014
    #define NUM_LABELS		5774
    #define NUM_OBJECT_CLASS 5996
   	#define SIM_SCORE_LIMIT 0.8 
   	#define NUM_ITERATIONS  5
   	#define VEC_DIM			300
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

    void readEdgeVector(vector<vector<double>> &edgeToVector){
    	int id;
    	double k;
    	ifstream fp("../data/labelEdge.txt");
    	while(fp>>id){
    		for(int i=0;i<VEC_DIM;i++){
    			fp>>k;
    			edgeToVector[id].push_back(k);
    		}
    	}

    	for(int i=0;i<VEC_DIM;i++){
    		edgeToVector[0].push_back(0);
    	}
    	fp.close();
    }

    void readNodeVector(vector<vector<double>> &nodeToVector){
    	int id;
    	double k;
    	ifstream fp("../data/vecNode.txt");
    	while(fp>>id){
    		for(int i=0;i<VEC_DIM;i++){
    			fp>>k;
    			nodeToVector[id].push_back(k);
    		}
    	}

    	for(int i=0;i<VEC_DIM;i++){
    		nodeToVector[0].push_back(0);
    	}
    	fp.close();
    }

	void readGraphs(vector<vector<pair<int,int>>> &sorted_nodes, vector<vector<int>> &nodes,vector<vector<int>> &edges, vector<vector<int>> &edge_u, vector<vector<int>> &edge_v){
    	char c;
    	int u,v,e,t;
		ifstream fp("../data/input.db");
		while(fp>>c){
	        switch(c){
	            case 't': fp>>c>>t;
	                      if(t>1){
	                          sort(sorted_nodes[t-1].begin(),sorted_nodes[t-1].end());
	                      }
	                      break;
	            case 'v': fp>>u>>v;
	                      nodes[t].push_back(v);
	                      sorted_nodes[t].push_back({v,u});
	                      break;
	            case 'e': fp>>u>>v>>e;
	                      edges[t].push_back(e);
	                      edge_u[t].push_back(u);
	                      edge_v[t].push_back(v);
	                      break;
	        }
	    }
	    fp.close();
	    sort(sorted_nodes[NUM_GRAPHS].begin(),sorted_nodes[NUM_GRAPHS].end());
	}

	vector<double> addVectors(vector<double> &a, vector<double> &b, vector<double> &c){
		vector<double> ans(VEC_DIM);
		for(int i=0;i<VEC_DIM;i++){
			ans[i] = a[i] + b[i] + c[i];
		}
		return ans;
	}

	double cosineSimilarity(vector<double> &a, vector<double> &b){
		long double len_a = 0.0, len_b = 0.0;
		long double dotProd = 0.0;
		for(int i=0;i<VEC_DIM;i++){
			dotProd += a[i]*b[i];
			len_a += a[i]*a[i];
			len_b += b[i]*b[i];
		}
		len_a = sqrt(len_a);
		len_b = sqrt(len_b);
		return dotProd/(len_a*len_b);
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

	double greedyMatchScore(vector<vector<double>> &costMatrix, double maxCost, vector<int> &perA, vector<int> &perB, vector<int> &node_mapping){
		int n = costMatrix.size();
		if(n < 1) return 0.0;
		int m = costMatrix[0].size();
		vector<int> randomOrder = generateRandom(n);
		vector<bool> vis(m,0);

		double cost = 0.0;
		for(int i=0;i<n;i++){
			int index = randomOrder[i]-1;
			double max_cost = -1;
			int maxIndex = -1;
			for(int j=0;j<m;j++){
				if(!vis[j] && costMatrix[index][j] > max_cost){
					max_cost = costMatrix[index][j];
					maxIndex = j;
				}
			}
			if(maxIndex != -1){
				vis[maxIndex] = 1;
				cost += max_cost;
			}
		}

		if(cost > maxCost){
			vector<bool> vis_col(m,0);
			for(int i=0;i<n;i++){
				int index = randomOrder[i]-1;
				double max_cost = -1;
				int maxIndex = -1;
				for(int j=0;j<m;j++){
					if(!vis_col[j] && costMatrix[index][j] > max_cost){
						max_cost = costMatrix[index][j];
						maxIndex = j;
					}
				}
				if(maxIndex != -1){
					vis_col[maxIndex] = 1;
				}
				node_mapping[perA[index]] = maxIndex==-1 ? maxIndex : perB[maxIndex];
			}
		}

		return max(cost,maxCost);
	}

	double nodeSimilarityScore(vector<pair<int,int>> &a, vector<pair<int,int>> &b, vector<int> &objectToLabel, vector<vector<double>> &pairSimScore, vector<int> &node_mapping){
		int n,m;
		n = a.size();
		m = b.size();
		int i=0,j=0;
		double commonNodes = 0;
		vector<int> distictNodesA,distinctNodesB,perA,perB;
		while(i<n && j<m){
			if(a[i].F == b[j].F){
				node_mapping[a[i].S] = b[j].S;
				commonNodes++;
				i++;
				j++;
			}
			else if(a[i].F < b[j].F){
				distictNodesA.push_back(a[i].F);
				perA.push_back(a[i].S);
				i++;
			}
			else{
				distinctNodesB.push_back(b[j].F);
				perB.push_back(b[j].S);
				j++;
			}
		}

		while(i<n){
			distictNodesA.push_back(a[i].F);
			perA.push_back(a[i].S);
			i++;
		}
		while(j<m){
			distinctNodesB.push_back(b[j].F);
			perB.push_back(b[j].S);
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
			maxCost = greedyMatchScore(costMatrix, maxCost, perA, perB, node_mapping);
		}
		perA.clear();
		perB.clear();
		return (commonNodes+maxCost);
	}

	double edgeSimilarityScore(vector<int> &a_u, vector<int> &a_v, vector<int> &a, vector<int> &b_u, vector<int> &b_v, vector<int> &b, vector<int> &node_mapping, vector<vector<double>> &nodeToVector, vector<vector<double>> &edgeToVector, vector<int> &nodes_a, vector<int> &nodes_b){
		map<pair<int,int>,vector<int>> mp_edges_b;
		int n = a.size();
		int m = b.size();
		for(int i=0;i<m;i++){
			mp_edges_b[{b_u[i],b_v[i]}].push_back(b[i]);
		}

		double totCost = 0.0;
		for(int i=0;i<n;i++){
			int u,v;
			u = a_u[i];
			v = a_v[i];
			if(mp_edges_b.find({node_mapping[u],node_mapping[v]})!=mp_edges_b.end()){
				vector<int> edge_label_list = mp_edges_b[{node_mapping[u],node_mapping[v]}];
				if(edge_label_list.size() == 0) continue;
				int edge_label = edge_label_list[0];
				mp_edges_b[{node_mapping[u],node_mapping[v]}].pop_back();
				int u1 = node_mapping[u];
				int v1 = node_mapping[v];
				vector<double> edgeVec1 = addVectors(nodeToVector[nodes_a[u]],nodeToVector[nodes_a[v]],edgeToVector[a[i]]);
				vector<double> edgeVec2 = addVectors(nodeToVector[nodes_b[u1]],nodeToVector[nodes_b[v1]],edgeToVector[edge_label]);

				totCost += cosineSimilarity(edgeVec1,edgeVec2);
				edgeVec1.clear();
				edgeVec2.clear();
			}
			else if(mp_edges_b.find({node_mapping[v],node_mapping[u]})!=mp_edges_b.end()){
				vector<int> edge_label_list = mp_edges_b[{node_mapping[u],node_mapping[v]}];
				if(edge_label_list.size() == 0) continue;
				int edge_label = edge_label_list[0];
				mp_edges_b[{node_mapping[u],node_mapping[v]}].pop_back();
				int u1 = node_mapping[u];
				int v1 = node_mapping[v];
				vector<double> edgeVec1 = addVectors(nodeToVector[nodes_a[u]],nodeToVector[nodes_a[v]],edgeToVector[a[i]]);
				vector<double> edgeVec2 = addVectors(nodeToVector[nodes_b[u1]],nodeToVector[nodes_b[v1]],edgeToVector[edge_label]);
				
				totCost += cosineSimilarity(edgeVec1,edgeVec2);
				edgeVec1.clear();
				edgeVec2.clear();
			}
		}
		return totCost;
	}

	void greedyMaximumBipartiteMatch(int start_i,int start_j,int end_i,int end_j,int threadId){
		string filename = "../Output/" + to_string(threadId) + ".txt";
		
		vector<vector<pair<int,int>>> sorted_nodes(NUM_GRAPHS + 1);
		vector<vector<int>> nodes(NUM_GRAPHS + 1);
    	vector<vector<int>> edges(NUM_GRAPHS + 1);
    	vector<vector<int>> edge_u(NUM_GRAPHS + 1);
    	vector<vector<int>> edge_v(NUM_GRAPHS + 1); 
		readGraphs(sorted_nodes, nodes, edges, edge_u, edge_v);

		vector<int> objectToLabel(NUM_OBJECT_CLASS + 1);
		readObjectId(objectToLabel);

		vector<vector<double>> edgeToVector(NUM_EDGE_CLASS + 1);
		readEdgeVector(edgeToVector);

		vector<vector<double>> nodeToVector(NUM_OBJECT_CLASS + 1);
		readNodeVector(nodeToVector);

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
					vector<int> node_mapping(nodes[i].size(),-1);
					if(totalNodesAndEdges > 0){
		    			double nodeSimScore = nodeSimilarityScore(sorted_nodes[i],sorted_nodes[j],objectToLabel,pairSimScore,node_mapping);
		            	double edgeSimScore = edgeSimilarityScore(edge_u[i],edge_v[i],edges[i],edge_u[j],edge_v[j],edges[j],node_mapping,nodeToVector,edgeToVector,nodes[i],nodes[j]);
		    			simScore = 2*(nodeSimScore + edgeSimScore)/totalNodesAndEdges;
		            }
		            
		            if(simScore >= SIM_SCORE_LIMIT){
		            	similarPairs.push_back({simScore,{i,j}});
		            }
		            node_mapping.clear();
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
    