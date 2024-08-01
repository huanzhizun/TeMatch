#include "tepattern.h"
#include <assert.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include "tegraph.h"

int TePattern::LoadData(std::string path){

    std::string pe=path+"edge.data";
    FILE* ffpe = fopen(pe.c_str(), "rb");
    assert(ffpe);

    int f=fread((char*)&originN, sizeof(uint32_t), 1, ffpe);
    f=fread((char*)&e, sizeof(uint32_t), 1, ffpe);
    printf("origin node: %d edge : %d \n", originN, e);

    in.reserve(originN);


    uint32_t *bufe = new uint32_t[e*2];
    f=fread((char*)bufe, sizeof(uint32_t), e*2, ffpe);

    nodeMap = new uint32_t[e+1];

    memset(nodeMap, 0, sizeof(uint32_t)*(e+1));
    
    fclose(ffpe);

    uint32_t *indg = new uint32_t[e+1];   //indgree
    memset(indg, 0, sizeof(uint32_t)*(e+1)); 
    uint32_t *adj = new uint32_t[originN*originN];
    memset(adj, 0, sizeof(uint32_t)*(originN*originN));

    auto t = get_wall_time();
    
    for(uint32_t i=0;i<e;i++){
        adj[bufe[2*i]*originN+bufe[2*i+1]]=1;
        indg[bufe[2*i+1]]++;
    }

    int root=0;
    for(uint32_t i=0;i<e;i++){
        if(indg[i]==0){
            root=i;
            break;
        }
    }

    std::queue<std::pair<uint32_t,uint32_t>> q;
    std::vector<std::pair<uint32_t,uint32_t>> edges;

    uint32_t *vis = new uint32_t[originN*originN];
    memset(vis, 0, sizeof(uint32_t)*(originN*originN));

    this->root = root;

    //do bfs to transfer DAG into Tree
    q.push(std::make_pair(root,root));
    int cnt=0;
    while(!q.empty()){
        auto now = q.front();q.pop();
        uint32_t from=now.first;
        uint32_t to=now.second;

        //not root
        if((from!=to||to!=root) && vis[nodeMap[from]*originN+to]==1) continue;

        vis[nodeMap[from]*originN+to]=1;

        if(from!=to||to!=root) edges.push_back(std::make_pair(from, cnt));
        nodeMap[cnt]=to;
        

        for(uint32_t i=0;i<originN;i++)
            if(adj[to*originN+i]) q.push(std::make_pair(cnt,i));
        cnt++;
    }

    delete []indg;
    delete []adj;
    delete []vis; 


    assert(cnt==e+1);

    std::sort(edges.begin(), edges.end());

    // printf("nodeMap: \n");
    // for(uint32_t i=0;i<cnt;i++){
    //     printf(" %d -> %d\n", i, nodeMap[i]);
    // }

    // printf("Transed tree: \n");
    for(int i=0;i<edges.size();i++){
        // printf("%d -> %d\n", edges[i].first, edges[i].second);
        bufe[2*i]=edges[i].first;
        bufe[2*i+1]=edges[i].second;
    }

    if(this->child){delete []this->child;this->child=nullptr;}
    if(this->fa){delete  []this->fa;this->fa=nullptr;}
    if(this->offset){delete []this->offset;this->offset=nullptr;}
    if(mode){delete []mode;mode=nullptr;}
    if(edgeIndx){delete []edgeIndx;edgeIndx=nullptr;}
    if(level){delete []level;level=nullptr;}
    if(inDgree){delete []inDgree;inDgree=nullptr;}
    if(pfrom){delete []pfrom;pfrom=nullptr;}
    if(pto){delete []pto;pto=nullptr;}
    if(maxOut||maxLevel) maxOut=maxLevel=0;

    n=e+1;

    child = new uint32_t[n+1];
    memset(child,INVALID,sizeof(uint32_t)*(n+1));
    fa = new uint32_t[n+1];
    memset(fa,INVALID,sizeof(uint32_t)*(n+1));
    offset = new uint32_t[n+1];
    memset(offset, INVALID ,(n+1)*sizeof(uint32_t));
    mode=new uint32_t[n*n+1];
    memset(mode,INVALID,sizeof(uint32_t)*(n*n+1));
    edgeIndx=new uint32_t[n*n+1];
    memset(edgeIndx,INVALID,sizeof(uint32_t)*(n*n+1));
    level=new uint32_t[n*n+1];
    memset(level,INVALID,sizeof(uint32_t)*(n*n+1));
    maxDepth=new uint32_t[n+1];
    memset(maxDepth,0,sizeof(uint32_t)*(n+1));
    inDgree=new uint32_t[originN+1];
    memset(inDgree,0,sizeof(uint32_t)*(originN+1));
    pfrom = new uint32_t[e+1];
    memset(pfrom,INVALID,sizeof(uint32_t)*(e+1));
    pto = new uint32_t[e+1];
    memset(pto,INVALID,sizeof(uint32_t)*(e+1));

    descendant.resize(n, std::vector<uint32_t>());

    uint32_t p=bufe[0];
    offset[p]=0;
    uint32_t tmpCnt=0;
    int *tmp = new int[n+1];
    memset(tmp, 0, (n+1)*sizeof(int));
    for(uint32_t i=0;i<e;i++){
        uint32_t from=bufe[2*i];
        uint32_t to=bufe[2*i+1];
        child[i]=to;
        fa[to]=from;
        level[(levelId(from, to))]=tmp[from];
        maxLevel = tmp[from]>maxLevel?tmp[from]:maxLevel;
        tmp[to] = tmp[from]+1;
        edgeIndx[outputId(from,to)]=tmpCnt++;

        // in[nodeMap[to]].push_back(nodeMap[from]);

        inDgree[nodeMap[to]]++;

        if(p!=from){
            p = from;
            offset[p]=i;
            maxOut=(offset[p]-offset[p-1])>maxOut?offset[p]-offset[p-1]:maxOut;
        }
    }

    maxLevel++;

    delete []tmp;
    assert(tmpCnt==e);
    // need to fix
    offset[n]=e;
    for(int i=n-1;i>=0;--i){
        if(offset[i]==INVALID) offset[i]=offset[i+1];
    }

    DescendantFunc(root);

    adjmat = new uint32_t*[originN];
    for(int i=0;i<originN;i++){
        adjmat[i] = new uint32_t[originN];
        memset(adjmat[i], INVALID, sizeof(uint32_t)*originN);
    }

    for(uint32_t i=0;i<e;i++){
        uint32_t from=bufe[2*i];
        uint32_t to=bufe[2*i+1];
        uint32_t outidx = outputId(from,to);
        adjmat[nodeMap[from]][nodeMap[to]] = edgeIndx[outidx];
        pfrom[edgeIndx[outidx]] = nodeMap[from];
        pto[edgeIndx[outidx]] = nodeMap[to];
    }

    auto t2=get_wall_time();
    delete []bufe;
    
    std::cout<<"loading Pattern costs: "<<t-t2<<" s, \nmaxOut: "<<maxOut<<"\nmaxLevel: "<<maxLevel<<"\n";
    return 1;
}

void TePattern::loadLabel(std::string path){

    if(label){delete []label;label=nullptr;}

    label = new uint32_t[n];
    memset(label, INVALID, sizeof(uint32_t)*n);

    std::string pl = path + "/label.data";
    int len = 0;
    FILE* fpl = fopen(pl.c_str(), "rb");

    assert(fpl);

    int f=fread((char*)&len, sizeof(uint32_t), 1, fpl);

    assert(len == originN);

    uint32_t *bufl = new uint32_t[len*2];
    memset(bufl, 0, sizeof(uint32_t)*(len*2));

    fread(bufl, sizeof(uint32_t), len*2, fpl);
    fclose(fpl);

    for(int i=0;i<len;i++){
        label[bufl[2*i]] = bufl[2*i+1];
    }

    delete[] bufl;

    return;
}

int TePattern::DescendantFunc(uint32_t node){
    uint32_t l,r;
    getChild(node, l, r);
    if(l==r){
        descendant[node].push_back(node);
    }
    else{
        for(uint32_t i=l;i<r;i++){
            DescendantFunc(child[i]);
            for(auto des: descendant[child[i]]) descendant[node].push_back(des);
        }
        // descendant[node].push_back(node);
    }
    return 1;
}

int TePattern::dfsMode(uint32_t vid, uint32_t &mmode){
    uint32_t l,r;
    getChild(vid,l,r);
    for(uint32_t i=l;i<r;i++){
        uint32_t ch=child[i];
        // this->mode[i]=mmode++;
        mode[modeId(vid,ch)]=mmode++;
        dfsMode(ch,mmode);
    }
    return 1;
}

int TePattern::automorphismCheck(uint32_t vid, uint32_t depth){
    uint32_t l,r;
    getChild(vid,l,r);
    if(l==r){
        maxDepth[vid]=1;
        return 1;
    }

    uint32_t md=0;
    for(uint32_t i=l;i<r;i++){
        uint32_t ch=child[i];
        automorphismCheck(ch, depth+1);
        md= (md<=maxDepth[ch])?maxDepth[ch]:md;
    }
    maxDepth[vid]=md+1;
    return 1;
}

int TePattern::automorphismMode(uint32_t root){
    int cnt=0;
    automorphismCheck(root, 0);
    std::unordered_map<uint32_t,uint32_t> hash;

    for(uint32_t i=0;i<n;i++){
        uint32_t cl,cr;
        getChild(i, cl, cr);
        for(uint32_t j=cl; j<cr; j++){
            uint32_t ch = child[j];
            uint32_t le = level[levelId(i,ch)];
            uint32_t mdj = maxDepth[ch];
            uint32_t key = ((uint16_t)le)<<16|mdj;
            if(hash.find(key)==hash.end()) hash[key]=cnt++;
            mode[modeId(i,ch)] = hash[key];
        }
    }

    return 1;
    
}


int TePattern::ScheduleGenerate(std::vector<uint32_t> &schedule){

    using namespace std;

    auto cmp = [](const pair<uint32_t, pair<uint32_t, uint32_t>>& a, const pair<uint32_t, pair<uint32_t, uint32_t>>& b) {
        
        uint32_t lea = a.second.first;
        uint32_t leb = b.second.first;
        uint32_t inda = a.second.second;
        uint32_t indb = b.second.second;

        if(lea==leb) return inda<indb;
        else return lea>leb;
    };

    priority_queue<pair<uint32_t, pair<uint32_t, uint32_t>>, vector<pair<uint32_t, pair<uint32_t, uint32_t>>>, decltype(cmp)> q(cmp);
    q.push(make_pair(root, make_pair(0,inDgree[root])));

    while(!q.empty()){
        auto p = q.top(); q.pop();
        uint32_t pid = p.first;
        uint32_t le = p.second.first;
        
        schedule.push_back(pid);

        uint32_t cl, cr;
        getChild(pid, cl, cr);
        for(uint32_t i=cl;i<cr;i++){
            uint32_t ch = child[i];
            q.push(make_pair(ch, make_pair(le+1,inDgree[nodeMap[ch]])));
        }
    }

    return 1;
}



void TePattern::edgeScheduleGenerate(std::vector<uint32_t> &schedule){

    char *vis = new char[e];
    memset(vis,0,sizeof(char)*e);

    // edgeScheFunc(schedule, root, vis);

    std::queue<uint32_t> q;

    q.push(root);
    while(!q.empty()){
        uint32_t n=q.front();q.pop();
        if(vis[n]) continue;
        vis[n]=1;
        for(int i=0;i<originN;i++){
            if(adjmat[n][i]!=INVALID){
                schedule.push_back(adjmat[n][i]);
                q.push(i);
            }
        }

    }

    delete[] vis;
}


void TePattern::getPaths(std::vector<std::vector<int>> &paths, std::vector<int>& edges, uint32_t root) const{

    uint32_t cl,cr;
    getChild(root, cl, cr);

    if(cl==cr){
        paths.push_back(edges);
        return;
    }

    for(uint32_t chid = cl; chid<cr;chid++){
        uint32_t ch = child[chid];
        edges.push_back(nodeMap[(int)ch]);
        getPaths(paths, edges, ch);
        edges.pop_back();
    }
}

void TePattern::ExtraPaths(std::vector<std::vector<int>> &extraPaths, const Schedule& schedule){

    std::vector<std::vector<int>> tpaths;
    std::vector<int> edges;

    edges.push_back(0);
    getPaths(tpaths, edges, 0);

    auto paths_temp = schedule.paths;

    for(auto extra: paths_temp){
        int inTPaths=0;
        for(auto p: tpaths){

            if(extra.size() != p.size()) continue;
            int match = 1;
            
            for(int i=0;i<extra.size();i++){
                if(extra[i] != p[i]){
                    match=0;
                    break;
                }
            }

            if(match){
                inTPaths = 1;
                break;
            }
        }
        if(inTPaths == 0) extraPaths.push_back(extra);
    }

}


bool TePattern::loadPattern(const Schedule& schedule, bool pathTree){
    
    originN = schedule.size;

    if(pathTree){
        int totN = 0;
        for(auto p: schedule.paths){
            totN += p.size()-1;
        }
        totN++;

        e = totN - 1;
    }
    else e = schedule.edges.size();

    printf("origin node: %d edge : %d \n", originN, e);

    in.reserve(originN);

    uint32_t *bufe = new uint32_t[e*2];

    if(pathTree == false){
        assert(e == schedule.edges.size());
        for(int i=0;i<e;i++){
            bufe[2*i] = schedule.edges[i].first;
            bufe[2*i+1] = schedule.edges[i].second;
        }
    }

    nodeMap = new uint32_t[e+1];

    memset(nodeMap, 0, sizeof(uint32_t)*(e+1));

    uint32_t *indg = new uint32_t[e+1];   //indgree
    memset(indg, 0, sizeof(uint32_t)*(e+1)); 
    uint32_t *adj = new uint32_t[originN*originN];
    memset(adj, 0, sizeof(uint32_t)*(originN*originN));

    auto t = get_wall_time();
    
    if(pathTree == false){
        for(uint32_t i=0;i<e;i++){
            adj[bufe[2*i]*originN+bufe[2*i+1]]=1;
            indg[bufe[2*i+1]]++;
        }
    }

    int root=0;
    for(uint32_t i=0;i<e;i++){
        if(indg[i]==0){
            root=i;
            break;
        }
    }

    std::queue<std::pair<uint32_t,uint32_t>> q;
    std::vector<std::pair<uint32_t,uint32_t>> edges;

    uint32_t *vis = new uint32_t[originN*originN];
    memset(vis, 0, sizeof(uint32_t)*(originN*originN));

    this->root = root;

    // * transfer pattern to path tree
    if(pathTree){
        int cnt=1;
        nodeMap[schedule.paths[0][0]] = 0;
        for(auto path: schedule.paths){
            for(int i=1;i<path.size();i++){
                int v = path[i];
                int u = path[i-1];
                nodeMap[cnt] = v;
                if(i==1) edges.push_back(std::make_pair(u, cnt));
                else edges.push_back(std::make_pair(cnt-1, cnt));
                cnt++;
            }
        }
        n = cnt;
    }
    else{
        // * do bfs to transfer DAG into Tree
        q.push(std::make_pair(root,root));
        int cnt=0;
        while(!q.empty()){
            auto now = q.front();q.pop();
            uint32_t from=now.first;
            uint32_t to=now.second;

            //not root
            if((from!=to||to!=root) && vis[nodeMap[from]*originN+to]==1) continue;

            vis[nodeMap[from]*originN+to]=1;

            if(from!=to||to!=root) edges.push_back(std::make_pair(from, cnt));
            nodeMap[cnt]=to;
            

            for(uint32_t i=0;i<originN;i++)
                if(adj[to*originN+i]) q.push(std::make_pair(cnt,i));
            cnt++;
        }
        n = e + 1;
    }

    printf("nodeMap:\n");
    for(int i=0;i<n;i++) printf("%d : %d\n", i, nodeMap[i]);

    delete []indg;
    delete []adj;
    delete []vis; 


    std::sort(edges.begin(), edges.end());
    assert(n == e+1);

    // printf("nodeMap: \n");
    // for(uint32_t i=0;i<cnt;i++){
    //     printf(" %d -> %d\n", i, nodeMap[i]);
    // }

    // printf("Transed tree: \n");
    for(int i=0;i<edges.size();i++){
        printf("%d -> %d\n", edges[i].first, edges[i].second);
        bufe[2*i]=edges[i].first;
        bufe[2*i+1]=edges[i].second;
    }

    if(this->child){delete []this->child;this->child=nullptr;}
    if(this->fa){delete  []this->fa;this->fa=nullptr;}
    if(this->offset){delete []this->offset;this->offset=nullptr;}
    if(mode){delete []mode;mode=nullptr;}
    if(edgeIndx){delete []edgeIndx;edgeIndx=nullptr;}
    if(level){delete []level;level=nullptr;}
    if(inDgree){delete []inDgree;inDgree=nullptr;}
    if(pfrom){delete []pfrom;pfrom=nullptr;}
    if(pto){delete []pto;pto=nullptr;}
    if(maxOut||maxLevel) maxOut=maxLevel=0;

    child = new uint32_t[n+1];
    memset(child,INVALID,sizeof(uint32_t)*(n+1));
    fa = new uint32_t[n+1];
    memset(fa,INVALID,sizeof(uint32_t)*(n+1));
    offset = new uint32_t[n+1];
    memset(offset, INVALID ,(n+1)*sizeof(uint32_t));
    mode=new uint32_t[n*n+1];
    memset(mode,INVALID,sizeof(uint32_t)*(n*n+1));
    edgeIndx=new uint32_t[n*n+1];
    memset(edgeIndx,INVALID,sizeof(uint32_t)*(n*n+1));
    level=new uint32_t[n*n+1];
    memset(level,INVALID,sizeof(uint32_t)*(n*n+1));
    maxDepth=new uint32_t[n+1];
    memset(maxDepth,0,sizeof(uint32_t)*(n+1));
    inDgree=new uint32_t[originN+1];
    memset(inDgree,0,sizeof(uint32_t)*(originN+1));
    pfrom = new uint32_t[e+1];
    memset(pfrom,INVALID,sizeof(uint32_t)*(e+1));
    pto = new uint32_t[e+1];
    memset(pto,INVALID,sizeof(uint32_t)*(e+1));

    descendant.resize(n, std::vector<uint32_t>());

    uint32_t p=bufe[0];
    offset[p]=0;
    uint32_t tmpCnt=0;
    int *tmp = new int[n+1];
    memset(tmp, 0, (n+1)*sizeof(int));
    for(uint32_t i=0;i<e;i++){
        uint32_t from=bufe[2*i];
        uint32_t to=bufe[2*i+1];
        child[i]=to;
        fa[to]=from;
        level[(levelId(from, to))]=tmp[from];
        maxLevel = tmp[from]>maxLevel?tmp[from]:maxLevel;
        tmp[to] = tmp[from]+1;
        edgeIndx[outputId(from,to)]=tmpCnt++;

        // in[nodeMap[to]].push_back(nodeMap[from]);

        inDgree[nodeMap[to]]++;

        if(p!=from){
            p = from;
            offset[p]=i;
            maxOut=(offset[p]-offset[p-1])>maxOut?offset[p]-offset[p-1]:maxOut;
        }
    }

    maxLevel++;

    delete []tmp;
    assert(tmpCnt==e);
    // need to fix
    offset[n]=e;
    for(int i=n-1;i>=0;--i){
        if(offset[i]==INVALID) offset[i]=offset[i+1];
    }

    DescendantFunc(root);

    adjmat = new uint32_t*[originN];
    for(int i=0;i<originN;i++){
        adjmat[i] = new uint32_t[originN];
        memset(adjmat[i], INVALID, sizeof(uint32_t)*originN);
    }

    for(uint32_t i=0;i<e;i++){
        uint32_t from=bufe[2*i];
        uint32_t to=bufe[2*i+1];
        uint32_t outidx = outputId(from,to);
        adjmat[nodeMap[from]][nodeMap[to]] = edgeIndx[outidx];
        pfrom[edgeIndx[outidx]] = nodeMap[from];
        pto[edgeIndx[outidx]] = nodeMap[to];
    }

    auto t2=get_wall_time();
    delete []bufe;
    
    std::cout<<"loading Pattern costs: "<<t2-t<<" s, \nmaxOut: "<<maxOut<<"\nmaxLevel: "<<maxLevel<<"\n";
    return 1;

    return true;
}