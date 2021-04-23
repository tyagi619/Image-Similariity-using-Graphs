// Run using the following command
// g++ mapillary.cpp -o output `pkg-config --cflags --libs opencv` -ljsoncpp

#include <bits/stdc++.h>
#include <dirent.h>
#include <opencv2/highgui.hpp>
#include <jsoncpp/json/json.h>

using namespace std;
using namespace cv;
using namespace Json;

ofstream outputGraph;
ofstream graphToImage;

class label{
    public:
        int r,g,b;
        int id;

        label(int item_id,int blue,int green,int red){
            id = item_id;
            r = red;
            g = green;
            b = blue;   
        }
};

long long bfs(int x,int y,int n,int m,int item_num,int **item,Mat &img){
    queue<pair<int,int>> q;
    q.push({x,y});
    item[x][y] = item_num;

    Vec3b color = img.at<Vec3b>(x,y);
    int rows[4] = {0,1,0,-1};
    int cols[4] = {-1,0,1,0};
    long long px_area = 0;
    while(!q.empty()){
        pair<int,int> p = q.front();
        q.pop();
        px_area++;
        for(int i=0;i<4;i++){
            int a = p.first + rows[i];
            int b = p.second + cols[i];
            if((a<0) || (b<0) || (a>=n) || (b>=m) || item[a][b]!=0){
                continue;
            }
            Vec3b px_color = img.at<Vec3b>(a,b);
            if(px_color==color){
                q.push({a,b});
                item[a][b] = item_num;
            }
        }
    }
    return px_area;
}

int process_image(string fname,int gid){
    Mat img;
    string relPath = "../Dataset/training/labels/" + fname;
    img = imread(relPath,IMREAD_COLOR);
    
    outputGraph<<"t # "<<gid<<endl;
    graphToImage<<gid<<" "<<fname<<endl;
    ifstream ifs("../Dataset/config.json");
    Reader reader;
    Value config;
    reader.parse(ifs,config);
    config = config["labels"];

    int r = img.rows;
    int c = img.cols;
    long long limit = (long long)(r)*(long long)c;
    limit*=1;
    limit/=1000;
    int channel = img.channels();
    
    int **item = new int*[r];
    for(int i=0;i<r;i++){
        item[i] = new int[c];
        for(int j=0;j<c;j++){
            item[i][j] = 0;
        }
    }
    vector<int> ignore;
    vector<int> vid;
    int item_count = 1;
    int vertex_id = 0;
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            if(item[i][j]==0){
                
                long long area = bfs(i,j,r,c,item_count++,item,img);
                if(area<=limit){
                    ignore.push_back(1);
                    vid.push_back(0);
                    continue;
                }
                ignore.push_back(0);

                Vec3b color = img.at<Vec3b>(i,j);
                label object(item_count,color[0],color[1],color[2]);
                
                for(int k=0;k<config.size();k++){
                    Value label_color = config[k]["color"];
                    if(label_color[0]==object.r && label_color[1]==object.g && label_color[2]==object.b){
                        object.id = config[k]["label_id"].asInt();
                        break;
                    }
                }
                vid.push_back(vertex_id);
                outputGraph<<"v "<<vertex_id<<" "<<object.id<<endl;
                vertex_id++;
            }
        }
    }

    bool **graph = new bool*[item_count];
    for(int i=0;i<item_count;i++){
        graph[i] = new bool[item_count];
        for(int j=0;j<item_count;j++){
            graph[i][j] = 0;
        }
    }
    int rows[4] = {1,0,-1,0};
    int cols[4] = {0,1,0,-1};
    for(int i=0;i<r;i++){
        for(int j=0;j<c;j++){
            for(int k=0;k<4;k++){
                int x = rows[k] + i;
                int y = cols[k] + j;
                if(x<0 || y<0 || x>=r || y>=c){
                    continue;
                }
                graph[item[i][j]][item[x][y]] = 1;
            }
        }
    }

    for(int i=1;i<item_count;i++){
        graph[i][i] = 0;
        if(ignore[i-1]){
            continue;
        }
        for(int j=1;j<item_count;j++){
            if(ignore[j-1]){
                continue;
            }
            if(graph[i][j]){
                outputGraph<<"e "<<vid[i-1]<<" "<<vid[j-1]<<" 1"<<endl;
                graph[j][i] = 0;
            }
        }
    }

    for(int i=0;i<item_count;i++){
        delete[] graph[i];
    }
    for(int i=0;i<r;i++){
        delete[] item[i];
    }
    delete[] item;
    delete[] graph;
    return 0;
}

int main(){
    int gid = 1;
    string name;
    outputGraph.open("../data/input.db");
    graphToImage.open("../data/graphToImage.txt");
    DIR *d;
    struct dirent *ent;
    if(d = opendir("../Dataset/training/labels/")){
        while ((ent = readdir (d)) != NULL) {
            string name(ent->d_name);
            if(name=="." || name=="..") continue;
            process_image(name,gid++);
        }
        closedir (d);
    }
    outputGraph.close();
    graphToImage.close();
    return 0;
}
