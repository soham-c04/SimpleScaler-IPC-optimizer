#include <stdlib.h>
#include <time.h>
#define N 20
#define M 50

int sz[N];
int list[N][N];
int mat[N][N];

void createGraph(){
    int u,v,w,m=M;
    while(m--){
        u = rand()%N;
        v = rand()%N;
        while((u==v) || mat[u][v]){ 
            u = rand()%N;
            v = rand()%N;
        }
        w = 1+rand()%50;
        w = 1; // Unit size
        mat[u][v] = mat[v][u] = w;
        list[u][sz[u]++] = v;
        list[v][sz[v]++] = u;
    }
}

void bfs(int dis[], int s){
    int i, l=0, r=0;
    int u,v;
    int queue[3*N];
    for(i=0;i<N;i++) dis[i] = -1;
    dis[s] = 0;
    queue[r++] = s;
    while(l<r){
        u = queue[l];
        l++;
        for(i=0;i<sz[u];i++){
            v = list[u][i];
            if(dis[v]==-1){
                dis[v] = dis[u]+1;
                queue[r++] = v;
            }
        }
    }
}

void main(){
    int dis[N];
    srand(1729);
    createGraph();
    bfs(dis, 0);
}