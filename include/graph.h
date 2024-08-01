#pragma once
#include "schedule.h"
#include "vertex_set.h"
#include <assert.h>
#include <unordered_map>
#include <stdint.h>

#include "config.h"

#include <bitset>
#include <vector>
#include <map>
#include <memory>

// #define UNDIRECT_GRAPH

// #define UNDIRECT_PATTERN

#define RENUMBER

// #define SHOW_VALIDATE

class Graphmpi;
class TeGraph;
class PathTree;


class Graph {
public:
    int v_cnt; // number of vertex
    unsigned int e_cnt; // number of edge
    long long tri_cnt; // number of triangle
    double max_running_time = 60 * 60 * 24; // second

    std::unordered_map<uint64_t, std::vector<std::pair<int, int>>> time;  // sorted by start and end ascend.
    std::shared_ptr<std::unordered_map<uint64_t, std::map<uint32_t, uint32_t>>> minimalArrive;  // sorted by end time and start time ascend. [uint64: u<<32|v] -> s,t
    std::unordered_map<uint64_t, std::unordered_map<uint32_t, uint32_t>> unorderArrive;  // sorted by end time and start time ascend.

    int max_intersection_size=-1;  // used in vertex init. should be set at LoadData()

    std::unique_ptr<long long[]> tot_vertex_cnt;

    int isTest;

    int *label;

    int DPfilter=0;  // 0 for brute_force filter 
                     // 1 for DP (minimalArrive) filter

    uint32_t *minimalTime;

    int *edge; // edges
    unsigned int *vertex; // v_i's neighbor is in edge[ vertex[i], vertex[i+1]-1]
    
    Graph() {
        v_cnt = 0;
        e_cnt = 0;
        edge = nullptr;
        vertex = nullptr;
        label = nullptr;
        isTest=0;
        minimalArrive = nullptr;
    }

    ~Graph() {
        if(edge != nullptr) delete[] edge;
        if(vertex != nullptr) delete[] vertex;
        if(label != nullptr) delete[] label;
    }

    long long calculate(const Schedule& schedule, VertexSet* vertex_set, int ans_size);

    int intersection_size(int v1,int v2);
    int intersection_size_clique(int v1,int v2);

    //single thread triangle counting
    long long triangle_counting();
    
    //multi thread triangle counting
    long long triangle_counting_mt(int thread_count);
    
    //general pattern matching algorithm with multi thread
    long long pattern_matching(const Schedule& schedule, int thread_count, bool clique = false);

    long long pattern_matching(const Schedule& schedule, int thread_count, const std::vector<int>& vec, bool clique = false);

    long long pattern_matching(const Schedule& schedule, unsigned int gentry, MaskPtr mask);

    //this function will be defined at code generation
    long long unfold_pattern_matching(const Schedule& schedule, int thread_count, bool clique = false);

    //general pattern matching algorithm with multi thread ans multi process
    long long pattern_matching_mpi(const Schedule& schedule, int thread_count, bool clique = false);

    void validate(const std::vector<int>& embed, int depth, long long& cnt, const Schedule& schedule, std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> &edges);

    void arriveValidate(const std::vector<int>& embed, long long& cnt, const Schedule& schedule);

    void validate(const VertexSet& subtraction_set, int lastEmbed, long long& cnt, const Schedule& schedule, int isTCG=0);

    bool validate(const std::vector<int>& embed, const PathTree& pathTree, const TeGraph& tg, int tgroot, int ptroot);

    bool pathValidate(const std::vector<int>& path, int depth, const Schedule& schedule, const std::pair<int, int>& tpair);

    bool pathValidate_reverse(const std::vector<int>& path, int depth, const Schedule& schedule, const std::pair<int, int>& tpair);

    int binarySearch(const std::vector<std::pair<int, int>>& vec, int t);

    void load(std::vector<std::pair<int,int>> edges);

    int max_degree;
public:
    friend Graphmpi;
    void tc_mt(long long * global_ans);

    void get_edge_index(int v, unsigned int& l, unsigned int& r) const;

    void pattern_matching_func(const Schedule& schedule, VertexSet* vertex_set, VertexSet& subtraction_set, long long& local_ans, int depth, bool clique = false);

    void pattern_matching_aggressive_func(const Schedule& schedule, VertexSet* vertex_set, VertexSet& subtraction_set, VertexSet& tmp_set, long long& local_ans, int depth, uint32_t *timeList=nullptr, MaskPtr mask=nullptr);

    //this function will be defined at code generation
    void unfold_pattern_matching_aggressive_func(const Schedule& schedule, VertexSet* vertex_set, VertexSet& subtraction_set, VertexSet& tmp_set, long long& local_ans, int depth);

};
