// Run using the following command
// g++ veo.cpp

#include <bits/stdc++.h>

using namespace std;

#define DATASET 18000

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
    vector<vector<int>> gVertex(DATASET+1);
    vector<vector<int>> gEdges(DATASET+1); 
    fp.open("../data/input.db");
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
                      if(gVertex[t][u] < gVertex[t][v]){
                          idx = (gVertex[t][u]-1)*66 + gVertex[t][v]-1;
                      }
                      else{
                          idx = (gVertex[t][v]-1)*66 + gVertex[t][u]-1;
                      }
                      gEdges[t].push_back(idx);
                      break;
        }
    }
    fp.close();
    int n = gVertex.size();
    sort(gVertex[DATASET].begin(),gVertex[DATASET].end());
    sort(gVertex[DATASET].begin(),gVertex[DATASET].end());
    

    ofstream ofp("../Output/result.txt");
    ofp<<fixed<<setprecision(3);
    for(int i=1;i<=DATASET;i++){
        cout<<i<<"\r";
        cout.flush();
        for(int j=i+1;j<=DATASET;j++){
            int tot = gVertex[i+1].size() + gVertex[j+1].size() + gEdges[i+1].size() + gEdges[j+1].size();
            int vertexIntsct = intersect(gVertex[i+1],gVertex[j+1]);
            int edgeIntsct = intersect(gEdges[i+1],gEdges[j+1]);
            long double sim = (long double)(2)*(long double)(vertexIntsct+edgeIntsct);
            if(tot > 0){
              sim = sim/tot;
            }
            else{
              sim = 0.0;
            }
            if(sim>=0.8){
                ofp<<i<<" "<<j<<" ";
                ofp<<sim<<endl;
            }
        }
    }
    ofp.close();
    cout<<endl;   
    return 0;
}
