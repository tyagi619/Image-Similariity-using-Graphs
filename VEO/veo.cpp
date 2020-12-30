// Run using the following command
// g++ veo.cpp

#include <bits/stdc++.h>

using namespace std;

#define endl "\n"
#define NUM_GRAPH 89946

int intersect(vector<int> a,vector<int> b){
    int n = a.size();
    int m = b.size();

    int i=0,j=0;
    int count = 0;
    while(i<n && j<m){
        if(a[i]<b[j]) i++;
        else if(a[i]>b[j]) j++;
        else{
            count++;
            i++;
            j++;
        }
    }
    return count;
}

int main(){
    ifstream fp;
    int t;
    char c;
    int u,v,e,idx;
    vector<vector<int>> gVertex(NUM_GRAPH + 1);
    vector<vector<int>> gEdges(NUM_GRAPH + 1); 
    fp.open("../GenGraph/input.db");
    while(fp>>c){
        switch(c){
            case 't': fp>>c>>t;
                      if(t>1){
                          sort(gVertex[t-1].begin(),gVertex[t-1].end());
                          sort(gEdges[t-1].begin(),gEdges[t-1].end());
                      }
                      break;
            case 'v': fp>>u>>v;
                      gVertex[t].push_back(v);
                      break;
            case 'e': fp>>u>>v>>e;
                      gEdges[t].push_back(e);
                      break;
        }
    }
    fp.close();
    sort(gVertex[NUM_GRAPH].begin(),gVertex[NUM_GRAPH].end());
    sort(gVertex[NUM_GRAPH].begin(),gVertex[NUM_GRAPH].end());
    
    cout<<"Data load complete\n\n";

    ofstream ofp("simIndex.txt");
    ofstream ofp1("simIndex_gte_60.txt");
    ofp<<fixed<<setprecision(3);
    ofp1<<fixed<<setprecision(3);
    for(int i=0;i<NUM_GRAPH;i++){
        
        cout<<(i+1)<<"\r";
        cout.flush();
        
        for(int j=i+1;j<NUM_GRAPH;j++){
            ofp<<i+1<<" : "<<j+1<<" --- ";
            int tot = gVertex[i+1].size() + gVertex[j+1].size() + gEdges[i+1].size() + gEdges[j+1].size();
            int vertexIntsct = intersect(gVertex[i+1],gVertex[j+1]);
            int edgeIntsct = intersect(gEdges[i+1],gEdges[j+1]);
            long double sim = (long double)(2)*(long double)(vertexIntsct+edgeIntsct);
            if(tot==0){
            	sim = 0;	
            } 
            else{
            	sim /= (tot);
            }
            ofp<<sim<<endl;
            if(sim>=0.6){
                ofp1<<i+1<<" : "<<j+1<<" --- ";
                ofp1<<sim<<endl;
            }
        }
    }
    cout<<endl;
    ofp.close();   
    ofp1.close(); 
    return 0;
}
