#include "../include/graph.h"
#include "../include/graphmpi.h"
#include "../include/vertex_set.h"
#include "../include/common.h"
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>
#include <omp.h>
#include <algorithm>
#include <cstring>
#include <mpi.h>
#include <atomic>
#include <queue>
#include <iostream>
#include "tegraph.h"
#include "pathtree.h"

int Graph::intersection_size(int v1,int v2) {
    unsigned int l1, r1;
    get_edge_index(v1, l1, r1);
    unsigned int l2, r2;
    get_edge_index(v2, l2, r2);
    int ans = 0;
    while(l1 < r1 && l2 < r2) {
        if(edge[l1] < edge[l2]) {
            ++l1;
        }
        else {
            if(edge[l2] < edge[l1]) {
                ++l2;
            }
            else {
                ++l1;
                ++l2;
                ++ans;
            }
        }
    }
    return ans;
}

/*int Graph::intersection_size_mpi(int v1, int v2) {
    Graphmpi &gm = Graphmpi::getinstance();
    int ans = 0;
    if (gm.include(v2))
        return intersection_size(v1, v2);
    unsigned int l1, r1;
    get_edge_index(v1, l1, r1);
    int *data = gm.getneighbor(v2);
    for (int l2 = 0; l1 < r1 && ~data[l2];) {
        if(edge[l1] < data[l2]) {
            ++l1;
        }
        else if(edge[l1] > data[l2]) {
            ++l2;
        }
        else {
            ++l1;
            ++l2;
            ++ans;
        }
    }
    return ans;
}
*/

int Graph::intersection_size_clique(int v1,int v2) {
    unsigned int l1, r1;
    get_edge_index(v1, l1, r1);
    unsigned int l2, r2;
    get_edge_index(v2, l2, r2);
    int min_vertex = v2;
    int ans = 0;
    if (edge[l1] >= min_vertex || edge[l2] >= min_vertex)
        return 0;
    while(l1 < r1 && l2 < r2) {
        if(edge[l1] < edge[l2]) {
            if (edge[++l1] >= min_vertex)
                break;
        }
        else {
            if(edge[l2] < edge[l1]) {
                if (edge[++l2] >= min_vertex)
                    break;
            }
            else {
                ++ans;
                if (edge[++l1] >= min_vertex)
                    break;
                if (edge[++l2] >= min_vertex)
                    break;
            }
        }
    }
    return ans;
}

long long Graph::triangle_counting() {
    long long ans = 0;
    for(int v = 0; v < v_cnt; ++v) {
        // for v in G
        unsigned int l, r;
        get_edge_index(v, l, r);
        for(unsigned int v1 = l; v1 < r; ++v1) {
            //for v1 in N(v)
            ans += intersection_size(v,edge[v1]);
        }
    }
    ans /= 6;
    return ans;
}

long long Graph::triangle_counting_mt(int thread_count) {
    long long ans = 0;
#pragma omp parallel num_threads(thread_count)
    {
        tc_mt(&ans);
    }
    return ans;
}

void Graph::tc_mt(long long *global_ans) {
    long long my_ans = 0;
    #pragma omp for schedule(dynamic)
    for(int v = 0; v < v_cnt; ++v) {
        // for v in G
        unsigned int l, r;
        get_edge_index(v, l, r);
        for(unsigned int v1 = l; v1 < r; ++v1) {
            if (v <= edge[v1])
                break;
            //for v1 in N(v)
            my_ans += intersection_size_clique(v,edge[v1]);
        }
    }
    #pragma omp critical
    {
        *global_ans += my_ans;
    }
}

void Graph::get_edge_index(int v, unsigned int& l, unsigned int& r) const
{
    // if(this->timeOffset==nullptr){
        l = vertex[v];
        r = vertex[v + 1];
    // }
    // else{
    //     int _nid = nid[v];
    //     l = vertex[v];
    //     r = vertex[timeOffset[_nid+1]];
    // }

}

// ! aborted: not used
void Graph::pattern_matching_func(const Schedule& schedule, VertexSet* vertex_set, VertexSet& subtraction_set, long long& local_ans, int depth, bool clique)
{
    int loop_set_prefix_id = schedule.get_loop_set_prefix_id(depth);
    int loop_size = vertex_set[loop_set_prefix_id].get_size();
    if (loop_size <= 0)
        return;
    int* loop_data_ptr = vertex_set[loop_set_prefix_id].get_data_ptr();
    /*if (clique == true)
      {
      int last_vertex = subtraction_set.get_last();
    // The number of this vertex must be greater than the number of last vertex.
    loop_start = std::upper_bound(loop_data_ptr, loop_data_ptr + loop_size, last_vertex) - loop_data_ptr;
    }*/
    if (depth == schedule.get_size() - 1)
    {
        // TODO : try more kinds of calculation.
        // For example, we can maintain an ordered set, but it will cost more to maintain itself when entering or exiting recursion.
        if (clique == true)
            local_ans += loop_size;
        else if (loop_size > 0)
            local_ans += VertexSet::unorderd_subtraction_size(vertex_set[loop_set_prefix_id], subtraction_set);
        return;
    }

    int last_vertex = subtraction_set.get_last();
    for (int i = 0; i < loop_size; ++i)
    {
        if (last_vertex <= loop_data_ptr[i] && clique == true)
            break;
        int vertex = loop_data_ptr[i];
        if (!clique)
            if (subtraction_set.has_data(vertex))
                continue;
        unsigned int l, r;
        get_edge_index(vertex, l, r);
        bool is_zero = false;
        for (int prefix_id = schedule.get_last(depth); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
        {
            vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], (int)r - l, prefix_id, this->max_intersection_size, vertex, clique);
            if( vertex_set[prefix_id].get_size() == 0) {
                is_zero = true;
                break;
            }
        }
        if( is_zero ) continue;
        //subtraction_set.insert_ans_sort(vertex);
        subtraction_set.push_back(vertex);
        pattern_matching_func(schedule, vertex_set, subtraction_set, local_ans, depth + 1, clique);
        subtraction_set.pop_back();
    }
}


long long Graph::pattern_matching(const Schedule& schedule, int thread_count, bool clique)
{
    long long global_ans = 0;
    double total_time = 0;
#pragma omp parallel num_threads(thread_count) reduction(+: global_ans, total_time)
    {
        double start_time = get_wall_time();
        double current_time;
        VertexSet* vertex_set = new VertexSet[schedule.get_total_prefix_num()];
        VertexSet subtraction_set;
        uint32_t timeList[schedule.size];
        VertexSet tmp_set;
        int sset_size = std::max(this->max_intersection_size, schedule.get_size());
        subtraction_set.init(sset_size);
        long long local_ans = 0;
        // TODO : try different chunksize

        memset(timeList, -1, sizeof(uint32_t)*schedule.size);


#pragma omp for schedule(dynamic) nowait
        for (int vertex = 0; vertex < v_cnt; ++vertex)
        {
            unsigned int l, r;
            get_edge_index(vertex, l, r);
            for (int prefix_id = schedule.get_last(0); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
            {
                vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], (int)r - l, prefix_id, this->max_intersection_size);
            }
            //subtraction_set.insert_ans_sort(vertex);
            subtraction_set.push_back(vertex);
            // if(DPfilter) timeList[0] = minimalTime[vertex];
            if(DPfilter) timeList[0] = 0;
            //if (schedule.get_total_restrict_num() > 0 && clique == false)
            if(true)
                pattern_matching_aggressive_func(schedule, vertex_set, subtraction_set, tmp_set, local_ans, 1, timeList);
            else
                pattern_matching_func(schedule, vertex_set, subtraction_set, local_ans, 1, clique);
            subtraction_set.pop_back();
            /*
            if( (vertex & (-vertex)) == (1<<15) ) {
                current_time = get_wall_time();
                if( current_time - start_time > max_running_time) {
                    printf("TIMEOUT!\n");
                    fflush(stdout);
                    assert(0);
                }
            }*/
        }

        delete[] vertex_set;
        // TODO : Computing multiplicty for a pattern
        global_ans += local_ans;
        total_time += get_wall_time()-start_time;
    }
    if(isTest == 0) printf("average_matchTime: %lf ", total_time/thread_count);
    return global_ans / schedule.get_in_exclusion_optimize_redundancy();
}

long long Graph::pattern_matching(const Schedule& schedule, int thread_count, const std::vector<int>& vec,bool clique)
{
    long long global_ans = 0;
#pragma omp parallel num_threads(thread_count) reduction(+: global_ans)
    {
        double start_time = get_wall_time();
        double current_time;
        VertexSet* vertex_set = new VertexSet[schedule.get_total_prefix_num()];
        VertexSet subtraction_set;
        uint32_t timeList[schedule.size];
        VertexSet tmp_set;
        int sset_size = std::max(this->max_intersection_size, schedule.get_size());
        subtraction_set.init(sset_size);
        long long local_ans = 0;
        // TODO : try different chunksize

        memset(timeList, -1, sizeof(uint32_t)*schedule.size);

#pragma omp for schedule(dynamic) nowait
        for (auto vertex: vec)
        {
            unsigned int l, r;
            get_edge_index(vertex, l, r);
            for (int prefix_id = schedule.get_last(0); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
            {
                vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], (int)r - l, prefix_id, this->max_intersection_size);
            }
            //subtraction_set.insert_ans_sort(vertex);
            subtraction_set.push_back(vertex);
            // if(DPfilter) timeList[0] = minimalTime[vertex];
            if(DPfilter) timeList[0] = 0;
            //if (schedule.get_total_restrict_num() > 0 && clique == false)
            if(true)
                pattern_matching_aggressive_func(schedule, vertex_set, subtraction_set, tmp_set, local_ans, 1, timeList);
            else
                pattern_matching_func(schedule, vertex_set, subtraction_set, local_ans, 1, clique);
            subtraction_set.pop_back();
            if(DPfilter) timeList[0] = -1;
            /*
            if( (vertex & (-vertex)) == (1<<15) ) {
                current_time = get_wall_time();
                if( current_time - start_time > max_running_time) {
                    printf("TIMEOUT!\n");
                    fflush(stdout);
                    assert(0);
                }
            }*/
        }

        delete[] vertex_set;
        // TODO : Computing multiplicty for a pattern
        global_ans += local_ans;
        
    }

    return global_ans / schedule.get_in_exclusion_optimize_redundancy();
}

long long Graph::pattern_matching(const Schedule& schedule, unsigned int gentry, MaskPtr mask)
{
    long long global_ans = 0;
    {
        double start_time = get_wall_time();
        double current_time;
        VertexSet* vertex_set = new VertexSet[schedule.get_total_prefix_num()];
        VertexSet subtraction_set;
        uint32_t timeList[schedule.size];
        VertexSet tmp_set;
        int sset_size = std::max(this->max_intersection_size, schedule.get_size());
        subtraction_set.init(sset_size);
        long long local_ans = 0;
        // TODO : try different chunksize

        memset(timeList, -1, sizeof(uint32_t)*schedule.size);

        unsigned int vertex = gentry;

        unsigned int l, r;
        get_edge_index(vertex, l, r);
        for (int prefix_id = schedule.get_last(0); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
        {
            vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], (int)r - l, prefix_id, this->max_intersection_size, -1, false, mask);
        }
        //subtraction_set.insert_ans_sort(vertex);
        subtraction_set.push_back(vertex);
        // if(DPfilter) timeList[0] = minimalTime[vertex];
        if(DPfilter) timeList[0] = 0;
        //if (schedule.get_total_restrict_num() > 0 && clique == false)

        pattern_matching_aggressive_func(schedule, vertex_set, subtraction_set, tmp_set, local_ans, 1, timeList, mask);

        subtraction_set.pop_back();
        if(DPfilter) timeList[0] = -1;

        delete[] vertex_set;
        // TODO : Computing multiplicty for a pattern
        global_ans += local_ans;
    }

    return global_ans / schedule.get_in_exclusion_optimize_redundancy();
}

void Graph::pattern_matching_aggressive_func(const Schedule& schedule, VertexSet* vertex_set, VertexSet& subtraction_set, VertexSet& tmp_set, long long& local_ans, int depth, uint32_t *timeList, MaskPtr mask) // 3 same # or @ in comment are useful in code generation ###
{
    int loop_set_prefix_id = schedule.get_loop_set_prefix_id(depth);// @@@
    int loop_size = vertex_set[loop_set_prefix_id].get_size();
    if (loop_size <= 0)
        return;

    int* loop_data_ptr = vertex_set[loop_set_prefix_id].get_data_ptr();
/* @@@ 
    //Case: in_exclusion_optimize_num = 2
    if (depth == schedule.get_size() - 2 && schedule.get_in_exclusion_optimize_num() == 2) { 
        int loop_set_prefix_id_nxt = schedule.get_loop_set_prefix_id( depth + 1);
        int loop_size_nxt = vertex_set[loop_set_prefix_id_nxt].get_size();
        int size1 = VertexSet::unorderd_subtraction_size(vertex_set[loop_set_prefix_id], subtraction_set);
        int size2 = VertexSet::unorderd_subtraction_size(vertex_set[loop_set_prefix_id_nxt], subtraction_set);
        VertexSet tmp_set;
        tmp_set.init();
        tmp_set.intersection(vertex_set[loop_set_prefix_id], vertex_set[loop_set_prefix_id_nxt]);
        int size3 = VertexSet::unorderd_subtraction_size(tmp_set, subtraction_set);
        local_ans += 1ll * size1 * size2 - size3;
        return;
    }
*/
/*
    //Case: in_exclusion_optimize_num = 3
    if( depth == schedule.get_size() - 3 && schedule.get_in_exclusion_optimize_num() == 3) { 
        int in_exclusion_optimize_num = 3;
        int loop_set_prefix_ids[ in_exclusion_optimize_num];
        for(int i = 0; i < in_exclusion_optimize_num; ++i)
            loop_set_prefix_ids[i] = schedule.get_loop_set_prefix_id( depth + i );
        
        int loop_sizes[ in_exclusion_optimize_num ];
        for(int i = 0; i < in_exclusion_optimize_num; ++i)
            loop_sizes[i] = VertexSet::unorderd_subtraction_size(vertex_set[loop_set_prefix_ids[i]], subtraction_set);
        
        local_ans += 1ll * loop_sizes[0] * loop_sizes[1] * loop_sizes[2];

        for(int i = 1; i < 3; ++i) 
            for(int j = 0; j < i; ++j){
                VertexSet tmp_set;
                tmp_set.init();
                tmp_set.intersection(vertex_set[loop_set_prefix_ids[i]], vertex_set[loop_set_prefix_ids[j]]);
                int tmp_size = VertexSet::unorderd_subtraction_size(tmp_set, subtraction_set);
                int size2;
                for(int k = 0; k < 3; ++k)
                    if( i != k && j != k) size2 = loop_sizes[k];
                local_ans -= 1ll * tmp_size * size2;
            }
        VertexSet tmp1;
        tmp1.init();
        tmp1.intersection(vertex_set[loop_set_prefix_ids[0]], vertex_set[loop_set_prefix_ids[1]]);
        VertexSet tmp2;
        tmp2.init();
        tmp2.intersection(vertex_set[loop_set_prefix_ids[2]], tmp1);
        local_ans += 1ll * 2 * VertexSet::unorderd_subtraction_size(tmp2, subtraction_set);
        return;
    }
*/
    //Case: in_exclusion_optimize_num > 1
    // if( depth == schedule.get_size() - schedule.get_in_exclusion_optimize_num() ) {
    //     int in_exclusion_optimize_num = schedule.get_in_exclusion_optimize_num();// @@@
    //     int loop_set_prefix_ids[ in_exclusion_optimize_num ];
    //     loop_set_prefix_ids[0] = loop_set_prefix_id;
    //     for(int i = 1; i < in_exclusion_optimize_num; ++i)
    //         loop_set_prefix_ids[i] = schedule.get_loop_set_prefix_id( depth + i );
    //     for(int optimize_rank = 0; optimize_rank < schedule.in_exclusion_optimize_group.size(); ++optimize_rank) {
    //         const std::vector< std::vector<int> >& cur_graph = schedule.in_exclusion_optimize_group[optimize_rank];
    //         long long val = schedule.in_exclusion_optimize_val[optimize_rank];
    //         for(int cur_graph_rank = 0; cur_graph_rank < cur_graph.size(); ++ cur_graph_rank) {
    //             //                VertexSet tmp_set;
                
    //             //if size == 1 , we will not call intersection(...)
    //             //so we will not allocate memory for data
    //             //otherwise, we need to copy the data to do intersection(...)
    //             if(cur_graph[cur_graph_rank].size() == 1) {
    //                 int id = loop_set_prefix_ids[cur_graph[cur_graph_rank][0]];
    //                 val = val * VertexSet::unorderd_subtraction_size(vertex_set[id], subtraction_set);
    //             }
    //             else {
    //                 int id0 = loop_set_prefix_ids[cur_graph[cur_graph_rank][0]];
    //                 int id1 = loop_set_prefix_ids[cur_graph[cur_graph_rank][1]];
    //                 tmp_set.init(this->max_degree);
    //                 tmp_set.intersection(vertex_set[id0], vertex_set[id1]);

    //                 for(int i = 2; i < cur_graph[cur_graph_rank].size(); ++i) {
    //                     int id = loop_set_prefix_ids[cur_graph[cur_graph_rank][i]];
    //                     tmp_set.intersection_with(vertex_set[id]);
    //                 }
    //                 val = val * VertexSet::unorderd_subtraction_size(tmp_set, subtraction_set);
    //             }
    //             if( val == 0 ) break;

    //         }
    //         local_ans += val;
    //     }
    //     return;// @@@
            
    // }
    //Case: in_exclusion_optimize_num <= 1
    if (depth == schedule.get_size() - 1)
    {
        // TODO : try more kinds of calculation. @@@
        // For example, we can maintain an ordered set, but it will cost more to maintain itself when entering or exiting recursion.
        int max_ans = vertex_set[loop_set_prefix_id].get_size();
        long long __local_ans = local_ans;

        std::vector<int> ans;
        ans.clear();
        int _ret=0;

        if (schedule.get_total_restrict_num() > 0)
        {
            int min_vertex = v_cnt;
            for (int i = schedule.get_restrict_last(depth); i != -1; i = schedule.get_restrict_next(i))
                if (min_vertex > subtraction_set.get_data(schedule.get_restrict_index(i)))
                    min_vertex = subtraction_set.get_data(schedule.get_restrict_index(i));
            const VertexSet& vset = vertex_set[loop_set_prefix_id];
            int size_after_restrict = std::lower_bound(vset.get_data_ptr(), vset.get_data_ptr() + vset.get_size(), min_vertex) - vset.get_data_ptr();
            if (size_after_restrict > 0)
                _ret = VertexSet::unorderd_subtraction_size_list(vertex_set[loop_set_prefix_id], subtraction_set,this->isTest, ans, size_after_restrict);
        }
        else
            _ret = VertexSet::unorderd_subtraction_size_list(vertex_set[loop_set_prefix_id],subtraction_set,this->isTest,ans); 

        // * return for restriction test
        if(isTest){
            local_ans += _ret;
            return;
        }
            
        if(ans.size()){
            int inTime[schedule.size];
            int outTime[schedule.size];

            if(tot_vertex_cnt != nullptr){
                int start_vertex = subtraction_set.get_data(0);
                tot_vertex_cnt[start_vertex] += calculate(schedule, vertex_set, ans.size());
            }

            for(auto lastEmbed: ans){
                // std::vector<int> vec(schedule.size);
                // for(int i=0;i<schedule.size;i++){
                //     int n = embed[i];
                //     vec[schedule.match_order[i]] = n;
                // }

                // for(auto n: vec) printf("%d ", n);
                // printf("\n");
                
                long long cnt = 0;

                // * only used for debug easier
                // std::vector<int> vec(schedule.size);
                // for(int i=0;i<schedule.size;i++){
                //     int oripid = schedule.match_order[i];
                //     if(i < subtraction_set.get_size())  vec[oripid] = subtraction_set.get_data(i);
                //     else vec[oripid] = lastEmbed;
                // }
                // for(auto i: vec) printf("%d ", i); 
                // printf("\n");

                
                // std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> edges;
                


                // edges.clear();

                // validate(embed, 0, cnt, schedule, edges);
                // validate(embed, cnt, schedule);
               
                if(DPfilter){
                    // auto t1 = get_wall_time();
                    int inVn = schedule.inVertexNum[depth];
                    int Valid = 1;
                    for(int _idx=0; _idx<inVn; _idx++){
                        int inV = schedule.inVertex[depth][_idx];
                        uint64_t temp = ((uint64_t)subtraction_set.get_data(inV))<<32 | lastEmbed;

                        assert(timeList[inV] != -1);
                        
                        auto lwb = (*minimalArrive)[temp].lower_bound(timeList[inV]);

                        if(lwb == (*minimalArrive)[temp].end()){
                            Valid = 0;
                            break;
                        }
                        
                    }

                    if(Valid) cnt = 1;
                    else cnt=0;
                    
                    // temporal_valid_time += get_wall_time() - t1;
                }
                else{
                    // std::vector<std::pair<std::pair<int,int>,std::pair<int,int>>> edges;
                    // edges.clear();
                    // validate(embed, 0, cnt, schedule, edges);
                    validate(subtraction_set, lastEmbed, cnt, schedule);
                }
                    
                    
                // assert(cnt==0 || cnt==1);
                local_ans += cnt;
                
                // * only used for debug temporal embeddings
                // if(cnt){
                //     std::vector<int> vec(schedule.size);
                //     for(int i=0;i<schedule.size;i++){
                //         int oripid = schedule.match_order[i];
                //         if(i < subtraction_set.get_size())  vec[oripid] = subtraction_set.get_data(i);
                //         else vec[oripid] = lastEmbed;
                //     }
                //     for(auto i: vec) printf("%d ", i); 
                //     printf("\n");
                // }
                
                // if(local_ans%10000 == 0) printf("%d\n", local_ans);
            }
        }
        return;// @@@
    }
  
    // TODO : min_vertex is also a loop invariant @@@
    int min_vertex = v_cnt;
    for (int i = schedule.get_restrict_last(depth); i != -1; i = schedule.get_restrict_next(i))
        if (min_vertex > subtraction_set.get_data(schedule.get_restrict_index(i)))
            min_vertex = subtraction_set.get_data(schedule.get_restrict_index(i));
    if (depth == 1) Graphmpi::getinstance().get_loop(loop_data_ptr, loop_size);
    int ii = 0;
    for (int &i = ii; i < loop_size; ++i)
    {
        if (min_vertex <= loop_data_ptr[i])
            break;
        int vertex = loop_data_ptr[i];
        if(mask != nullptr && mask->test(vertex) == false) continue;
        if (subtraction_set.has_data(vertex))
            continue;

        uint32_t t=-1;
        // auto t1 = get_wall_time();
        if(DPfilter){
            int inVn = schedule.inVertexNum[depth];
            int Valid = 1;
            for(int _idx=0; _idx<inVn; _idx++){
                int inV = schedule.inVertex[depth][_idx];
                uint64_t temp = ((uint64_t)subtraction_set.get_data(inV))<<32 | vertex;

                assert(timeList[inV] != -1);

                // if(unorderArrive[temp].find(timeList[inV]) == unorderArrive[temp].end()){
                    
                    auto lwb = (*minimalArrive)[temp].lower_bound(timeList[inV]);

                    if(lwb == (*minimalArrive)[temp].end()){
                        Valid = 0;
                        break;
                    }
                    else{
                        if(t==(uint32_t)-1) t = lwb->second;
                        else t = t>lwb->second ? t : lwb->second;
                    }
                    
                // }
                // else{
                //     uint32_t tarr = unorderArrive[temp][timeList[inV]];
                //     if(t==(uint32_t)-1) t = tarr;
                //     else t = t>tarr ? t : tarr;
                // }


            }

            if(Valid == 0) continue;
            
        }
        // temporal_valid_time += get_wall_time() - t1;

        unsigned int l, r;
        get_edge_index(vertex, l, r);
        bool is_zero = false;
        for (int prefix_id = schedule.get_last(depth); prefix_id != -1; prefix_id = schedule.get_next(prefix_id))
        {
            vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, &edge[l], (int)r - l, prefix_id, this->max_intersection_size,  vertex, false, mask);
            if( vertex_set[prefix_id].get_size() == 0) {
                is_zero = true;
                break;
            }
        }
        if( is_zero ) continue;

        //subtraction_set.insert_ans_sort(vertex);
        if(DPfilter) timeList[depth] = t;
        subtraction_set.push_back(vertex);
        pattern_matching_aggressive_func(schedule, vertex_set, subtraction_set, tmp_set, local_ans, depth + 1, timeList, mask);// @@@
        subtraction_set.pop_back(); // @@@
        if(DPfilter) timeList[depth] = -1;
    } 
    //if (depth == 1 && ii < loop_size) Graphmpi::getinstance().set_cur(subtraction_set.get_data(0));// @@@
} 
// ###

void Graph::arriveValidate(const std::vector<int>& embed, long long& cnt, const Schedule& schedule){

    int Valid = 1;

    for(auto path: schedule.paths){

        if(path.size()<=2) continue;

        std::vector<int> p;
        p.clear();

        for(auto i: path){
            p.push_back(embed[schedule.output_order[i]]);
        }

        uint64_t temp = ((uint64_t)p[0])<<32 | p[1];

        int arrive = (*minimalArrive)[temp].begin()->first;  // first start time

        for(int i=1; i<p.size(); i++){

            uint32_t u = (uint32_t)p[i-1];
            uint32_t v = (uint32_t)p[i];

            uint64_t temp = ((uint64_t)u)<<32 | v;

            auto lwb = (*minimalArrive)[temp].lower_bound(arrive);

            if(lwb == (*minimalArrive)[temp].end()){
                Valid = 0;
                break;
            }
            else arrive = lwb->second;
            
        }

        if(Valid == 0) break;
    }


    if(Valid) cnt=1;
    else cnt=0;

}

void Graph::validate(const VertexSet& subtraction_set, int lastEmbed, long long& cnt, const Schedule& schedule, int isTCG){

    int f = 0;

    std::vector<std::vector<int>> tempath;

    tempath = schedule.paths;

#ifdef SHOW_VALIDATE
    printf("--------------------------\n");
    printf("embed: ");
    for(int i=0;i<embed.size();i++) printf("%d ", embed[schedule.output_order[i]]);
    printf("\n");
    printf("--------------------------\n");
#endif

    for(auto path: tempath){

        if(path.size()<=2) continue;

        std::vector<int> p;
        p.clear();

        for(auto i: path){
            int index = schedule.output_order[i];
            if(index < subtraction_set.get_size())
                p.push_back(subtraction_set.get_data(index));
            else p.push_back(lastEmbed);
        }

        bool ret;

        // if(isTCG) ret = pathValidate_reverse(p, p.size()-1, schedule, std::make_pair(0,0));
        // else ret = pathValidate(p, 1, schedule, std::make_pair(0,0));
#ifdef SHOW_VALIDATE
        printf("Path: ");
        for(auto i:path) printf("%d ", i);
        printf("as: (");
        for(auto i:p) printf("%d ", id[i]);
        printf(")\n");
#endif
        ret = pathValidate(p, 1, schedule, std::make_pair(0,0));
#ifdef SHOW_VALIDATE
        printf("Path: ");
        for(auto i:path) printf("%d ", i);
        printf("return %d\n", ret);
#endif
        if(ret==false){
            f = 1;
            break;
        }
    }

    if(f) cnt = 0;
    else cnt = 1;

#ifdef SHOW_VALIDATE
    printf("embed: ");
    for(int i=0;i<embed.size();i++) printf("%d ", embed[schedule.output_order[i]]);
    printf("return %d\n", cnt);
#endif

}

bool Graph::pathValidate(const std::vector<int>& path, int depth, const Schedule& schedule, const std::pair<int, int>& tpair){

    if(depth == path.size()){
        return true;
    }
    else{
        uint32_t u = path[depth-1];
        uint32_t v = path[depth];

        uint64_t temp = ((uint64_t)u)<<32 | v;
        // for(auto e: path){
        //     printf("%d ", id[e]);
        // }
        // printf("\n");
        // printf("%d %d\n", id[u], id[v]);
        assert(time[temp].size());

        // int ret = binarySearch(time[temp], tpair.second);

        // if(ret == time[temp].size()){

        //     return false;
        // }
        // else{
        for(int idx=0; idx<time[temp].size(); idx++){

            auto tp = time[temp][idx];

            if(tp.first < tpair.second) continue;

            int ret = pathValidate(path, depth+1, schedule, tp);

            if(ret){

                return true;
            }
        }
            return false;
        // }
    }

}

int Graph::binarySearch(const std::vector<std::pair<int, int>>& vec, int t) {
    int left = 0;
    int right = vec.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (vec[mid].first < t) { 
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return left;
}

long long Graph::calculate(const Schedule& schedule, VertexSet* vertex_set, int ans_size){
    long long tot_vertex_size = 0;
    for(int i=1;i<schedule.get_size();i++){
        int loop_set_prefix_id = schedule.get_loop_set_prefix_id(i);
        int loop_size = 0;
        if(vertex_set[loop_set_prefix_id].mask != nullptr){
            int size = vertex_set[loop_set_prefix_id].get_size();
            for(int j=0; j<size; j++){
                int data = vertex_set[loop_set_prefix_id].get_data(j);
                if(vertex_set[loop_set_prefix_id].mask->test(data) == false) continue;
                loop_size++;
            }
        }
        else loop_size = vertex_set[loop_set_prefix_id].get_size();
        tot_vertex_size += loop_size;
    }
    tot_vertex_size += ans_size;
    return tot_vertex_size;
}


long long Graph::pattern_matching_mpi(const Schedule& schedule, int thread_count, bool clique)
{
    Graphmpi &gm = Graphmpi::getinstance();
    long long global_ans = 0;
#pragma omp parallel num_threads(thread_count)
    {
#pragma omp master
        {
            gm.init(thread_count, this, schedule);
        }
#pragma omp barrier //mynodel have to be calculated before running other threads
#pragma omp master
        {
            global_ans = gm.runmajor();
        }
        if (omp_get_thread_num()) {
            VertexSet* vertex_set = new VertexSet[schedule.get_total_prefix_num()];
            long long local_ans = 0;
            VertexSet subtraction_set;
            VertexSet tmp_set;
            int sset_size = std::max(this->max_intersection_size, schedule.get_size());
            subtraction_set.init(sset_size);
            int last = -1;
            gm.set_loop_flag();
            auto match_edge = [&](int vertex, int *data, int size) {
                if (vertex != last) {
                    if (~last) subtraction_set.pop_back();
                    unsigned int l, r;
                    get_edge_index(vertex, l, r);
                    for (int prefix_id = schedule.get_last(0); prefix_id != -1; prefix_id = schedule.get_next(prefix_id)) {
                        vertex_set[prefix_id].build_vertex_set(schedule, vertex_set, edge + l, r - l, prefix_id, this->max_intersection_size);
                    }
                    subtraction_set.push_back(vertex);
                    last = vertex;
                }
                gm.set_loop(data, size);
                pattern_matching_aggressive_func(schedule, vertex_set, subtraction_set, tmp_set, local_ans, 1);
            };
            for (unsigned int *data; data = gm.get_edge_range();) {
                match_edge(data[1], edge + data[2], data[3] - data[2]);
                /*for (int i = 1; i <= data[4]; i++) {
                    int l, r;
                    get_edge_index(data[1] + i, l, r);
                    match_edge(data[1] + i, edge + l, r - l);
                }*/
            }
            delete[] vertex_set;
            gm.report(local_ans);
        }
    }
    return global_ans;
}
