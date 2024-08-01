#include "../include/dataloader.h"
#include "../include/graph.h"
#include "../include/vertex_set.h"
#include "../include/common.h"
#include <cstdlib>
#include <cstring>
#include <unordered_set>


bool DataLoader::load_data(Graph* &g, DataType type, std::string path, int oriented_type, int LoadType) {
    if(type == Patents || type == Orkut || type == complete8 || type == LiveJournal || type == MiCo || type == CiteSeer || type == Wiki_Vote|| type== SELF_DEFINED) {
        return general_load_data(g, type, path, oriented_type, LoadType);
    }
    printf("invalid DataType!\n");
    return false;
}

bool DataLoader::general_load_data(Graph *&g, DataType type, std::string path, int oriented_type, int LoadType) {
    std::string staticG = path+"static.txt";

    if (freopen(staticG.c_str(), "r", stdin) == NULL)
    {
        printf("File not found. %s\n", staticG.c_str());
        return false;
    }
    printf("Load begin in %s\n",staticG.c_str());
    g = new Graph();

    g->isTest=0;

    //load triangle counting information
    switch(type) {
        case DataType::Patents : {
            g->tri_cnt = Patents_tri_cnt;
            break;
        }
        case DataType::LiveJournal : {
            g->tri_cnt = LiveJournal_tri_cnt;
            break;
        }
        case DataType::MiCo : {
            g->tri_cnt = MiCo_tri_cnt;
            break;
        }
        case DataType::CiteSeer : {
            g->tri_cnt = CiteSeer_tri_cnt;
            break;
        }
        case DataType::Wiki_Vote : {
            g->tri_cnt = Wiki_Vote_tri_cnt;
            break;
        }
        case DataType::Orkut : {
            g->tri_cnt = Orkut_tri_cnt;
            break;
        }
        case DataType::SELF_DEFINED:{
            g->tri_cnt = Self_def_cnt;
            break;
        }
        default : {
            g->tri_cnt = -1;
            break;
        }
    }

    scanf("%d%u",&g->v_cnt,&g->e_cnt);
    int* degree = new int[g->v_cnt];
    memset(degree, 0, g->v_cnt * sizeof(int));
#ifdef UNDIRECT_GRAPH
    g->e_cnt *= 2;
#endif
    std::pair<int,int> *e = new std::pair<int,int>[g->e_cnt];
    std::map<int,int> id;
    id.clear();
    int x,y;
    int tmp_v;
    unsigned int tmp_e;
    tmp_v = 0;
    tmp_e = 0;
    while(scanf("%d%d",&x,&y)!=EOF) {
        if(x == y) {
            printf("find self circle\n");
            g->e_cnt -=2;
            continue;
            //return false;
        }
        if(!id.count(x)){
            id[x] = x;
            tmp_v++;
        }
        if(!id.count(y)){
            id[y] = y;
            tmp_v++;
        }

        e[tmp_e++] = std::make_pair(x,y);
        ++degree[x];
#ifdef UNDIRECT_GRAPH
        e[tmp_e++] = std::make_pair(y,x);
        ++degree[y];
#endif
        //if(tmp_e % 1000000u == 0u) {
        //    printf("load %u edges\n",tmp_e);
        //    fflush(stdout);
        //}
    }

    std::sort(degree, degree + g->v_cnt);

    // The max size of intersections is the second largest degree.
    //TODO VertexSet::max_intersection_size has different value with different dataset, but we use a static variable now.
    // VertexSet::max_intersection_size = std::max( VertexSet::max_intersection_size, degree[g->v_cnt - 2]);
    g->max_intersection_size = std::max( g->max_intersection_size, degree[g->v_cnt - 2]);
    g->max_degree = degree[g->v_cnt - 1];
    delete[] degree;
    if(tmp_v != g->v_cnt) {
        printf("vertex number error!, tmp_v: %d, v_cnt: %d\n", tmp_v, g->v_cnt);
        for(int i=0;i<g->v_cnt;i++){
            if(!id.count(i)) printf("not find %d\n", i);
        }
    }
    if(tmp_e != g->e_cnt) {
        printf("edge number error!, tmp_e: %d, e_cnt: %d\n",tmp_e, g->e_cnt);
    }
    if(tmp_v != g->v_cnt || tmp_e != g->e_cnt) {
        fclose(stdin);
        delete g;
        delete[] e;
        return false;
    }
    std::sort(e,e+tmp_e,cmp_pair);
    // ! need to unique the edge set
    g->e_cnt = unique(e,e+tmp_e) - e;
    for(unsigned int i = 0; i < g->e_cnt - 1; ++i)
        if(e[i] == e[i+1]) {
            printf("have same edge\n");
            fclose(stdin);
            delete g;
            delete[] e;
            return false;
        }
    g->edge = new int[g->e_cnt];
    g->vertex = new unsigned int[g->v_cnt + 1];
    bool* have_edge = new bool[g->v_cnt];
    int lst_v = -1;
    for(int i = 0; i < g->v_cnt; ++i) have_edge[i] = false;
    for(unsigned int i = 0; i < g->e_cnt; ++i) {
        if(e[i].first != lst_v) {
            have_edge[e[i].first] = true;
            g->vertex[e[i].first] = i;
        }
        lst_v = e[i].first;
        g->edge[i] = e[i].second;
    }
    delete[] e;
    printf("Success! There are %d nodes and %u edges.\n",g->v_cnt,g->e_cnt);
    fflush(stdout);
    g->vertex[g->v_cnt] = g->e_cnt;
    for(int i = g->v_cnt - 1; i >= 0; --i)
        if(!have_edge[i]) {
            g->vertex[i] = g->vertex[i+1];
        }
    delete[] have_edge;

    if(LoadType == 0) 
        return true;

    std::string ptime = path+"temporal.txt";
    if (freopen(ptime.c_str(), "r", stdin) == NULL)
    {
        printf("File not found. %s\n", ptime.c_str());
        return false;
    }

    int u,v,s,t;
    while(scanf("%d%d%d%d",&u,&v,&s,&t)!=EOF) {
#ifdef RENUMBER
        uint64_t temp = ((uint64_t)id[u])<<32 | id[v];
#else
        uint64_t temp = ((uint64_t)u)<<32 | v;
#endif
        g->time[temp].push_back(std::make_pair(s,t));
    }

    for(auto _t: g->time){
        uint64_t temp = _t.first;
        std::sort(g->time[temp].begin(), 
        g->time[temp].end(), 
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) -> bool {
            if(a.first == b.first) return a.second<b.second;
            return a.first < b.first;
        });
    }

    return true;
}

bool DataLoader::load_complete(Graph* &g, int clique_size) {
    g = new Graph();

    g->v_cnt = clique_size;
    g->e_cnt = clique_size * (clique_size - 1) / 2;

    g->isTest=1;

    int* degree = new int[g->v_cnt];
    memset(degree, 0, g->v_cnt * sizeof(int));
    g->e_cnt *= 2;
    std::pair<int,int> *e = new std::pair<int,int>[g->e_cnt];

    std::map<int,int> id;
    id.clear();
    int tmp_v;
    unsigned int tmp_e;
    tmp_v = 0;
    tmp_e = 0;
    for(int i = 0; i < clique_size; ++i)
        for(int j = 0; j < i; ++j) {
            int x = i, y = j;
            if(!id.count(x)) id[x] = tmp_v ++;
            if(!id.count(y)) id[y] = tmp_v ++;
            x = id[x];
            y = id[y];
            e[tmp_e++] = std::make_pair(x,y);
            e[tmp_e++] = std::make_pair(y,x);
            ++degree[x];
            ++degree[y];
        }

    std::sort(degree, degree + g->v_cnt);

    // The max size of intersections is the second largest degree.
    //TODO VertexSet::max_intersection_size has different value with different dataset, but we use a static variable now.
    // VertexSet::max_intersection_size = std::max( VertexSet::max_intersection_size, degree[g->v_cnt - 2]);
    g->max_intersection_size = std::max( g->max_intersection_size, degree[g->v_cnt - 2]);
    g->max_degree = degree[g->v_cnt - 1];
    delete[] degree;
    if(tmp_v != g->v_cnt) {
        printf("vertex number error!\n");
    }
    if(tmp_e != g->e_cnt) {
        printf("edge number error!\n");
    }
    if(tmp_v != g->v_cnt || tmp_e != g->e_cnt) {
        fclose(stdin);
        delete g;
        delete[] e;
        return false;
    }
    std::sort(e,e+tmp_e,cmp_pair);
    g->e_cnt = unique(e,e+tmp_e) - e;
    g->edge = new int[g->e_cnt];
    g->vertex = new unsigned int[g->v_cnt + 1];
    bool* have_edge = new bool[g->v_cnt];
    int lst_v = -1;
    for(int i = 0; i < g->v_cnt; ++i) have_edge[i] = false;
    for(unsigned int i = 0; i < g->e_cnt; ++i) {
        if(e[i].first != lst_v) {
            have_edge[e[i].first] = true;
            g->vertex[e[i].first] = i;
        }
        lst_v = e[i].first;
        g->edge[i] = e[i].second;
    }
    delete[] e;
    g->vertex[g->v_cnt] = g->e_cnt;
    for(int i = g->v_cnt - 1; i >= 0; --i)
        if(!have_edge[i]) {
            g->vertex[i] = g->vertex[i+1];
        }
    delete[] have_edge;
    return true;
}

bool DataLoader::cmp_pair(std::pair<int,int>a, std::pair<int,int>b) {
    return a.first < b.first || (a.first == b.first && a.second < b.second);
}

bool DataLoader::cmp_degree_gt(std::pair<int,int> a,std::pair<int,int> b) {
    return a.second > b.second;
}

bool DataLoader::cmp_degree_lt(std::pair<int,int> a,std::pair<int,int> b) {
    return a.second < b.second;
}

long long DataLoader::comb(int n, int k) {
    long long ans = 1;
    for(int i = n; i > n - k; --i)
        ans = ans * i;
    for(int i = 1; i <= k; ++i)
        ans = ans / k;
    return ans;
}


bool DataLoader::load_data(Graph* &g, const std::vector<std::pair<uint32_t, uint32_t>>& edges, int oriented_type){

    g = new Graph();

    g->isTest=0;

    std::unordered_set<int> set;
    set.clear();

    g->v_cnt=0;
    g->e_cnt=edges.size();

    for(int i=0;i<edges.size();i++){
        int x = edges[i].first;
        int y = edges[i].second;
        if(set.find(x) == set.end()){
            g->v_cnt++;
            set.insert(x);
        }
        if(set.find(y) == set.end()){
            g->v_cnt++;
            set.insert(y);
        }
    }

    int* degree = new int[g->v_cnt];
    memset(degree, 0, g->v_cnt * sizeof(int));
#ifdef UNDIRECT_GRAPH
    g->e_cnt *= 2;
#endif
    std::pair<int,int> *e = new std::pair<int,int>[g->e_cnt];
    std::map<int,int> id;

    id.clear();
    unsigned int tmp_e;
    tmp_e = 0;

    for(auto _e: edges){
        int x = _e.first;
        int y = _e.second;

        if(x == y) {
            printf("find self circle\n");
            g->e_cnt -=2;
            continue;
            //return false;
        }
        if(!id.count(x)){
            id[x] = x;
        }
        if(!id.count(y)){
            id[y] = y;
        }

        e[tmp_e++] = std::make_pair(x,y);
        ++degree[x];
#ifdef UNDIRECT_GRAPH
        e[tmp_e++] = std::make_pair(y,x);
        ++degree[y];
#endif
        //if(tmp_e % 1000000u == 0u) {
        //    printf("load %u edges\n",tmp_e);
        //    fflush(stdout);
        //}
    }

    std::sort(degree, degree + g->v_cnt);

    // The max size of intersections is the second largest degree.
    //TODO VertexSet::max_intersection_size has different value with different dataset, but we use a static variable now.
    // VertexSet::max_intersection_size = std::max( VertexSet::max_intersection_size, degree[g->v_cnt - 2]);
    g->max_intersection_size = std::max( g->max_intersection_size, degree[g->v_cnt - 2]);
    g->max_degree = degree[g->v_cnt - 1];
    delete[] degree;
    for(int i=0;i<g->v_cnt;i++){
        if(!id.count(i)){
            printf("not find %d\n", i);
            delete g;
            delete[] e;
            return false;
        }

    }
    if(tmp_e != g->e_cnt) {
        printf("edge number error!, tmp_e: %d, e_cnt: %d\n",tmp_e, g->e_cnt);
        fclose(stdin);
        delete g;
        delete[] e;
        return false;
    }
    std::sort(e,e+tmp_e,cmp_pair);
    // ! need to unique the edge set
    g->e_cnt = unique(e,e+tmp_e) - e;
    for(unsigned int i = 0; i < g->e_cnt - 1; ++i)
        if(e[i] == e[i+1]) {
            printf("have same edge\n");
            fclose(stdin);
            delete g;
            delete[] e;
            return false;
        }
    g->edge = new int[g->e_cnt];
    g->vertex = new unsigned int[g->v_cnt + 1];
    bool* have_edge = new bool[g->v_cnt];
    int lst_v = -1;
    for(int i = 0; i < g->v_cnt; ++i) have_edge[i] = false;
    for(unsigned int i = 0; i < g->e_cnt; ++i) {
        if(e[i].first != lst_v) {
            have_edge[e[i].first] = true;
            g->vertex[e[i].first] = i;
        }
        lst_v = e[i].first;
        g->edge[i] = e[i].second;
    }
    delete[] e;
    // printf("Success! There are %d nodes and %u edges.\n",g->v_cnt,g->e_cnt);
    fflush(stdout);
    g->vertex[g->v_cnt] = g->e_cnt;
    for(int i = g->v_cnt - 1; i >= 0; --i)
        if(!have_edge[i]) {
            g->vertex[i] = g->vertex[i+1];
        }
    delete[] have_edge;

    return true;

}
