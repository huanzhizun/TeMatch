#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"
#include "../include/motif_generator.h"
#include "tegraph.h"
#include "tepattern.h"
#include "pathtree.h"
#include "vertex_set.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
#include <fstream>

#include <omp.h>

#include <stack>
#include <tuple>

#include <mutex>

std::mutex print_mutex;

void write_csv(const std::unique_ptr<long long[]>& array, int size, const std::string& filename) {
    std::ofstream file(filename);

    // 检查文件是否成功打开
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return;
    }

    // 写入 CSV 表头
    file << "id,cnt\n";

    // 遍历数组并写入非零元素
    for (int i = 0; i < size; ++i) {
        if (array[i] != 0) {  // 检查元素是否非零
            file << i << "," << array[i] << "\n";  // 写入索引和元素值
        }
    }

    // 关闭文件
    file.close();
}

void getMinimalRoot(TeGraph* tg, const std::vector<uint32_t>& root, std::vector<uint32_t>& minimal){

    uint32_t lastnid=-1, lasttime=0;
    for(auto i: root){
        if(lastnid!=tg->nid(i)){
            lastnid=tg->nid(i);
            lasttime=tg->time(i);
            minimal.push_back(i);
        }
        else{
            if(tg->time(i)<lasttime){
                minimal.pop_back();
                minimal.push_back(i);
            }
        }
    }
}

void TCG(TePattern *tp, TeGraph *tg, uint32_t pentry, uint32_t gentry, std::unordered_set<uint64_t>& tedgeset, std::unordered_set<uint64_t>& edgeset, uint8_t *visited){

    // last vertex on timelist g
    uint32_t gtl, gtr;
    tg->getTimeList(tg->nid(gentry), gtl, gtr);

    uint32_t cl,cr;
    tp->getChild(pentry, cl, cr);
    for(uint32_t chid = cl; chid<cr; chid++){
        uint32_t pnext = tp->child[chid];
        uint32_t validMode = tp->mode[tp->modeId(pentry, pnext)];
        uint32_t tvid = gtr-1;
        while(tvid!=INVALID && tvid >= gentry){
            uint32_t strid = tg->storeId(tvid, validMode);
            if(visited[strid]){
                tvid--;
                continue;
            }
            uint32_t canNum = tg->candidateNum[strid];
            if(tg->vis[strid] && canNum !=0){
                for(int i=0;i<canNum;i++){
                    uint32_t canId = tg->candidateId(tvid,validMode, i);
                    uint32_t nxtvid = tg->candidate[canId];
                    uint64_t tedge = (((uint64_t)tvid)<<32)|nxtvid;
                    uint64_t edge = (((uint64_t)tg->nid(tvid))<<32)|tg->nid(nxtvid);
                    tedgeset.insert(tedge);
                    edgeset.insert(edge);
                    uint32_t pnxtl, pnxtr;
                    if(tp->getChild(pnext, pnxtl, pnxtr)) TCG(tp, tg, pnext, nxtvid, tedgeset, edgeset, visited);
                }
            }
            visited[strid]=1;
            tvid--;
        }
    }
}

std::vector<std::pair<uint32_t, uint32_t>> TCG_iter(TePattern *tp, TeGraph *tg, uint32_t gentry, uint8_t *visited){

    std::vector<std::pair<uint32_t, uint32_t>> edges;

    uint32_t pentry = 0;

    std::stack<std::tuple<uint32_t, uint32_t, uint32_t>> stk;
    stk.emplace(pentry, gentry, tg->nid(gentry));

    while (!stk.empty()) {
        auto [pentry, gentry, gentry_nid] = stk.top();
        stk.pop();

        // last vertex on timelist g
        uint32_t gtl, gtr;
        tg->getTimeList(gentry_nid, gtl, gtr);

        uint32_t cl, cr;
        tp->getChild(pentry, cl, cr);
        for (uint32_t chid = cl; chid < cr; chid++) {
            uint32_t pnext = tp->child[chid];
            uint32_t validMode = tp->mode[tp->modeId(pentry, pnext)];
            uint32_t tvid = gtr - 1;
            while (tvid != INVALID && tvid >= gentry) {
                uint32_t strid = tg->storeId(tvid, validMode);
                if (visited[strid]) {
                    tvid--;
                    continue;
                }
                uint32_t canNum = tg->candidateNum[strid];
                if (tg->vis[strid] && canNum != 0) {
                    for (int i = 0; i < canNum; i++) {
                        uint32_t canId = tg->candidateId(tvid, validMode, i);
                        uint32_t nxtvid = tg->candidate[canId];
                        edges.emplace_back(tg->nid(tvid), tg->nid(nxtvid));
                        uint32_t pnxtl, pnxtr;
                        if (tp->getChild(pnext, pnxtl, pnxtr)) {
                            stk.emplace(pnext, nxtvid, tg->nid(nxtvid));
                        }
                    }
                }
                visited[strid] = 1;
                tvid--;
            }
        }
    }

    return std::move(edges);  // move construction rather than copy construction
}

void TCG_masks(TePattern *tp, TeGraph *tg, uint32_t gentry, uint8_t *visited, std::shared_ptr<std::bitset<MAX_MASK_SIZE>> mask){

    uint32_t pentry = 0;

    std::stack<std::tuple<uint32_t, uint32_t, uint32_t>> stk;
    mask->set(tg->nid(gentry));
    stk.emplace(pentry, gentry, tg->nid(gentry));

    while (!stk.empty()) {
        auto [pentry, gentry, gentry_nid] = stk.top();
        stk.pop();

        // last vertex on timelist g
        uint32_t gtl, gtr;
        tg->getTimeList(gentry_nid, gtl, gtr);

        uint32_t cl, cr;
        tp->getChild(pentry, cl, cr);
        for (uint32_t chid = cl; chid < cr; chid++) {
            uint32_t pnext = tp->child[chid];
            uint32_t validMode = tp->mode[tp->modeId(pentry, pnext)];
            uint32_t tvid = gtr - 1;
            while (tvid != INVALID && tvid >= gentry) {
                uint32_t strid = tg->storeId(tvid, validMode);
                // if (visited[strid]) {
                //     tvid--;
                //     continue;
                // }
                uint32_t canNum = tg->candidateNum[strid];
                if (tg->vis[strid] && canNum != 0) {
                    for (int i = 0; i < canNum; i++) {
                        uint32_t canId = tg->candidateId(tvid, validMode, i);
                        uint32_t nxtvid = tg->candidate[canId];

                        mask->set(tg->nid(nxtvid));
                        uint32_t pnxtl, pnxtr;
                        if (tp->getChild(pnext, pnxtl, pnxtr)) {
                            stk.emplace(pnext, nxtvid, tg->nid(nxtvid));
                        }
                    }
                }
                // visited[strid] = 1;
                tvid--;
            }
        }
    }

}



int main(int argc,char *argv[]) {

    if(argc < 5) {
        printf("Usage: %s dataset_name graph_file pattern_size pattern_adjacency_matrix thread_cnt\n", argv[0]);
        printf("Example(Triangle counting on dataset WikiVote) : \n");
        printf("%s Wiki-Vote( or SELF_DEFINED) ../../dataset/wiki-vote_input 3 011101110 24\n", argv[0]);
        return 0;
    }
    

    const std::string type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];
    int thread_count = atoi(argv[5]);

    // comments in include/schedule.h explain the meaning of these parameters.
    int test_type = 1; // performance_modeling_type = restricts_type = use_in_exclusion_optimize = 1
    int no_use_in_exclusion_optimize = 0;
    int no_use_restricts=0;

    DataType my_type;
    
    GetDataType(my_type, type);

    if(my_type == DataType::Invalid) {
        printf("Dataset not found!\n");
        return 0;
    }

    Graph *g;
    DataLoader D;

    Pattern *p;
    p = new Pattern(size, adj_mat);
    
    TeGraph *tg = new TeGraph();
    tg->LoadData(path);

    std::vector<std::pair<uint32_t, uint32_t>> edge;
    for(auto e: tg->staticEdges){
        edge.push_back(std::make_pair(e>>32, e&0xffffffff));
    }

    assert(D.load_data(g, edge));
    g->tot_vertex_cnt = std::make_unique<long long[]>(g->v_cnt);
    std::memset(g->tot_vertex_cnt.get(), 0, g->v_cnt * sizeof(long long));

    int use_performance_model = 1, use_restricts = 1, no_use_IEP = 0;

    bool is_pattern_valid;
    Schedule schedule(*p, is_pattern_valid, use_performance_model, use_restricts, no_use_IEP, 0,0,0);
    assert(is_pattern_valid);

    schedule.print_schedule();
    const auto& pairs = schedule.restrict_pair;
    printf("%d ",pairs.size());
    for(auto& p : pairs)
        printf("(%d,%d)",p.first,p.second);
    puts("");
    fflush(stdout);


    TePattern *tp = new TePattern();
    tp->loadPattern(schedule);

    uint32_t m=0;
    tp->dfsMode(0, m);

    // tp->automorphismMode(0);

    std::vector<uint32_t> root;
    double tcs1 = get_wall_time();
    tg->match(tp, g, root);
    double tcs2 = get_wall_time();

    std::vector<uint32_t> minimalRoot;
    getMinimalRoot(tg, root, minimalRoot);

    schedule.tg = tg;

    std::shared_ptr<std::unordered_map<uint64_t, std::map<uint32_t, uint32_t>>> 
    minimalArrive = std::make_shared<std::unordered_map<uint64_t, std::map<uint32_t, uint32_t>>>();

    for(auto u : tg->minimalArrive){
        uint32_t uid = u.first;
        for(auto v: u.second){
            uint32_t vid = v.first;
            uint64_t temp = (((uint64_t)uid)<<32)|vid;

            if((*minimalArrive).find(temp) == (*minimalArrive).end()){
                (*minimalArrive)[temp] = std::map<uint32_t, uint32_t>();
                g->unorderArrive[temp] = std::unordered_map<uint32_t, uint32_t>();
            }

            for(auto p: v.second){
                (*minimalArrive)[temp][p.first] = p.second;
                g->unorderArrive[temp][p.first] = p.second;
            }
        }
    }

    g->minimalArrive = minimalArrive;

    g->DPfilter = 1;

    int root_size = minimalRoot.size();
    int originRoot[root_size];
    memset(originRoot, -1, sizeof(int)*root_size);
    for(int rootId=0; rootId < root_size; rootId++){
        uint32_t gentry = minimalRoot[rootId];
        originRoot[rootId] = tg->nid(gentry);
    }

    double TCStime=tcs2-tcs1;
    double TCGtime=0;
    double MatchTime=0;
    long long totAns=0;

    printf("thread: %d\n", thread_count);

    double tend1 = get_wall_time();
    
#pragma omp parallel num_threads(thread_count) reduction(+: totAns, TCGtime, MatchTime) 
{

    // uint8_t *visited = new uint8_t[tg->maxPattern*tg->tnodeCnt];


#pragma omp for schedule(dynamic) nowait
    for(int rootId=0; rootId<root_size; rootId++)
    {
        uint32_t vertex = originRoot[rootId];
        // uint32_t gentry = minimalRoot[rootId];
        // memset(visited, 0, sizeof(uint8_t)*(tg->maxPattern*tg->tnodeCnt));

        // auto mask = std::make_shared<std::bitset<MAX_MASK_SIZE>>();

        // double tcg1 = get_wall_time();
        // TCG_masks(tp, tg, gentry, nullptr, mask);
        // double tcg2 = get_wall_time();

        double tm1 = get_wall_time();
        long long ans = g->pattern_matching(schedule, vertex, nullptr);
        double tm2 = get_wall_time();

        // TCGtime += (tcg2-tcg1);
        MatchTime += (tm2-tm1);
        totAns += ans;
    }

    // delete []visited;
}

    double tend2 = get_wall_time();

    double end_matchTime = tend2-tend1;
    double end2end = end_matchTime + TCStime;

    double ave_matchTime = MatchTime/thread_count;


    printf("root %d ans %ld TCStime %.6lf Matchtime %.6lf tot %.6lf end2end %.6lf\n", minimalRoot.size(), totAns, TCStime, ave_matchTime, TCStime+ave_matchTime, end2end);

    // write_csv(g->tot_vertex_cnt, g->v_cnt, "icpp_result/calculate/unit_test/p8_5.csv");

    
    delete tp;
    delete p;
    delete tg;
    delete g;

}


