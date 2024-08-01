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

void check_size(TeGraph* tg, const std::vector<uint32_t>& roots){
    printf("first loop size: %d\n",roots.size());
    return;
}

void test_pattern(Graph* g, TeGraph* tg, const Pattern &pattern, int performance_modeling_type, int restricts_type, int thread_cnt, bool use_in_exclusion_optimize = false) {
    int thread_num = thread_cnt;
    // int thread_num = 1;
    double t1,t2;
    
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize,0, 0, 0);
    assert(is_pattern_valid);

    // return;

    TePattern *tp = new TePattern();
    tp->loadPattern(schedule);

    uint32_t m=0;
    tp->dfsMode(0, m);
    // tp->automorphismMode(0);

    // printf("temporal pattern mode check:\n");
    // for(uint32_t i=0;i<tp->n;i++)
    //     for(uint32_t j=0;j<tp->n;j++){

    //         if(i==1 && j==3 && tp->mode[tp->modeId(i,j)]==2) tp->mode[tp->modeId(i,j)]=5;

    //         if(tp->mode[tp->modeId(i,j)]!=INVALID) printf("(%d,%d): %d\n", i, j, tp->mode[tp->modeId(i,j)]);
            
    //     }
    

    std::vector<uint32_t> root;
    double tcs1 = get_wall_time();
    tg->match(tp, g, root);
    double tcs2 = get_wall_time();

    std::vector<uint32_t> minimal_root;
    minimal_root.clear();
    getMinimalRoot(tg, root, minimal_root);

    check_size(tg, minimal_root);
    

    return;

}

double getMB(unsigned long long bytes){
    return bytes/1024.0/1024.0;
}

typedef std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::map<uint32_t, uint32_t>>> ComplexMap;

// Function to calculate memory used by the innermost std::map
size_t calculateInnerMapMemory(const std::map<uint32_t, uint32_t>& innerMap) {
    size_t size = sizeof(innerMap);  // Initial overhead of the map
    for (const auto& kv : innerMap) {
        size += sizeof(kv.first) + sizeof(kv.second);
    }
    return size;
}

// Function to calculate memory used by the std::unordered_map containing std::map
size_t calculateMidMapMemory(const std::unordered_map<uint32_t, std::map<uint32_t, uint32_t>>& midMap) {
    size_t size = sizeof(midMap);  // Initial overhead of the unordered_map
    for (const auto& kv : midMap) {
        size += sizeof(kv.first) + calculateInnerMapMemory(kv.second);
    }
    return size;
}

// Function to calculate total memory usage of the complex map structure
size_t calculateComplexMapMemory(const ComplexMap& complexMap) {
    size_t size = sizeof(complexMap);  // Initial overhead of the outer unordered_map
    for (const auto& kv : complexMap) {
        size += sizeof(kv.first) + calculateMidMapMemory(kv.second);
    }
    return size;
}


void getMemory(TeGraph* tg){

    double total_mem = 0.0;

    total_mem = getMB(sizeof(uint32_t)*(tg->tedgeCnt)+sizeof(uint64_t)*tg->tnodeCnt);
    total_mem += getMB(sizeof(uint32_t)*tg->tnodeCnt);

    unsigned long long size = 0;

    for(int i=0; i<tg->tnodeCnt*tg->maxPattern;i++)
        if(tg->next[i] != INVALID) size++;

    total_mem += getMB(size);

    size = 0;
    for(int i=0; i<tg->tnodeCnt*tg->maxPattern*tg->maxOut;i++)
        if(tg->candidate[i] != INVALID) size++;

    total_mem += getMB(size);

    size = calculateComplexMapMemory(tg->minimalArrive);

    total_mem += getMB(size);

    printf("total_mem MB: %lf\n", total_mem);
}



int main(int argc,char *argv[]) {

    if(argc < 6) {
        printf("Usage: %s dataset_name graph_file pattern_size pattern_adjacency_matrix thread\n", argv[0]);
        printf("Example(Triangle counting on dataset WikiVote) : \n");
        printf("%s Wiki-Vote( or SELF_DEFINED) ../../dataset/wiki-vote_input 3 011101110 thread_cnt\n", argv[0]);
        return 0;
    }
    

    const std::string type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];

    int thread_cnt = atoi(argv[5]);

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

    int dont_load_temporal_info = 0;
    int no_orient = 0;

    // assert(D.load_data(g,my_type,path, no_orient, dont_load_temporal_info)==true); 

    Pattern *p;
    p = new Pattern(size, adj_mat);

    TeGraph *tg = new TeGraph();
    tg->LoadData(path);

    std::vector<std::pair<uint32_t, uint32_t>> edge;
    for(auto e: tg->staticEdges){
        edge.push_back(std::make_pair(e>>32, e&0xffffffff));
    }

    assert(D.load_data(g, edge));
    g->minimalTime=nullptr;
    g->tot_vertex_cnt = std::make_unique<long long[]>(g->v_cnt);
    std::memset(g->tot_vertex_cnt.get(), 0, g->v_cnt * sizeof(long long));

    // int tot_out_node = 0;
    // for(int i=0; i<g->v_cnt; i++){
    //     unsigned int l,r;
    //     g->get_edge_index(i, l, r);
    //     if(r-l >= 6) tot_out_node++;
    // }
    // printf("v_cnt: %d, out_v_cnt: %d\n", g->v_cnt, tot_out_node);

    // return 1;

    g->minimalTime = new uint32_t[g->v_cnt];
    memset(g->minimalTime, -1, sizeof(uint32_t)*g->v_cnt);
    
    for(uint32_t i=0; i<tg->nodeCnt; i++){
        uint32_t tl, tr, id, time;
        tg->getTimeList(i, tl, tr);
        tg->getInfo(tl, id, time);
        g->minimalTime[i] = time;
    }
    
    test_pattern(g, tg, *p, test_type, 1, thread_cnt,no_use_in_exclusion_optimize);

    // getMemory(tg);

    // write_csv(g->tot_vertex_cnt, g->v_cnt, "icpp_result/calculate/baseline/p8_5.csv");

    delete tg;
    delete p;
    delete g;

}
