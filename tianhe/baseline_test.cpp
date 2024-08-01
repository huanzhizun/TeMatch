#include <../include/graph.h>
#include <../include/dataloader.h>
#include "../include/pattern.h"
#include "../include/schedule.h"
#include "../include/common.h"
#include "../include/motif_generator.h"

#include <assert.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

void test_pattern(Graph* g, const Pattern &pattern, int performance_modeling_type, int restricts_type, bool use_in_exclusion_optimize = false, int thread_num = 24) {
    // int thread_num = 1;
    double t1,t2;
    
    bool is_pattern_valid;
    Schedule schedule(pattern, is_pattern_valid, performance_modeling_type, restricts_type, use_in_exclusion_optimize, 0,0,0);
    assert(is_pattern_valid);

    schedule.print_schedule();
    const auto& pairs = schedule.restrict_pair;
    printf("%d ",pairs.size());
    for(auto& p : pairs)
        printf("(%d,%d)",p.first,p.second);
    puts("");
    fflush(stdout);

    g->DPfilter = 0;
    printf("running thread: %d\n", thread_num);

    t1 = get_wall_time();
    long long ans = g->pattern_matching(schedule, thread_num);
    t2 = get_wall_time();

    printf("ans %lld time %.6lf\n", ans, t2 - t1);

}

int main(int argc,char *argv[]) {

    if(argc < 5) {
        printf("Usage: %s dataset_name graph_file pattern_size pattern_adjacency_matrix label\n", argv[0]);
        printf("Example(Triangle counting on dataset WikiVote) : \n");
        printf("%s Wiki-Vote( or SELF_DEFINED) ../../dataset/wiki-vote_input 3 011101110 [labels]\n", argv[0]);
        return 0;
    }

    Graph *g;
    DataLoader D;

    const std::string type = argv[1];
    const std::string path = argv[2];
    
    int size = atoi(argv[3]);
    char* adj_mat = argv[4];
    
    int thread_num = atoi(argv[5]);

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

    assert(D.load_data(g,my_type,path)==true); 
    // assert(D.load_data(g,my_type,path.c_str())==true); 

    // return 1;

    printf("Load data success!\n");
    fflush(stdout);

    // int restrict_type = 0;
    Pattern *p;
    p = new Pattern(size, adj_mat);
    
    test_pattern(g, *p, test_type, 1, no_use_in_exclusion_optimize, thread_num);
    
    delete g;
    delete p;
}
