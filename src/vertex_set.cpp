#include "../include/vertex_set.h"
#include <algorithm>
#include <cstring>
#include <assert.h>
#include "graph.h"
#include <unordered_set>


VertexSet::VertexSet()
:data(nullptr), size(0), allocate(false), mask(nullptr)
{}

void VertexSet::init(const Graph& g, MaskPtr mask)
{
    assert(g.max_intersection_size != -1);
    if (allocate == true && data != nullptr)
        size = 0; // do not reallocate
    else
    {
        size = 0;
        allocate = true;
        data = new int[g.max_intersection_size];
    }
    this->mask = mask;
}

//this function is only used for tmp_set in graph.cpp (i.e., init(Graph.max_degree))
void VertexSet::init(int input_size, MaskPtr mask)
{
    assert(input_size > 0);
    // printf("allocate vertex size : %d\n", input_size);
    if (allocate == true && data != nullptr)
        size = 0;
    else
    {
        size = 0;
        allocate = true;
        data = new int[input_size];
    }
    this->mask = mask;
}

void VertexSet::init(int input_size, int* input_data, MaskPtr mask)
{
    if (allocate == true && data != nullptr)
        delete[] data;
    size = input_size;
    data = input_data;
    allocate = false;
    this->mask = mask;
}

void VertexSet::copy(int input_size, const int* input_data, MaskPtr mask)
{
    size = input_size;
    for(int i = 0; i < input_size; ++i) data[i] = input_data[i];
    this->mask = mask;
}

VertexSet::~VertexSet()
{
    if (allocate== true && data != nullptr)
        delete[] data;
    if(mask) mask = nullptr;
}

void VertexSet::intersection(const VertexSet& set0, const VertexSet& set1, int min_vertex, bool clique)
{
    if (&set0 == &set1)
    {
        copy(set0.get_size(), set0.get_data_ptr(), set0.mask);
        return;
    }
    int i = 0;
    int j = 0;
    int size0 = set0.get_size();
    int size1 = set1.get_size();

    // TODO : Try more kinds of calculation.
    // Like
    // while (true)
    //     ..., if (++i == size0) break;
    //     ..., if (++j == size1) break;
    //     ......
    // Maybe we can also use binary search if one set is very small and another is large.
    if (clique == true)
        if (set0.get_data(0) >= min_vertex || set1.get_data(0) >= min_vertex)
            return;
    int data0 = set0.get_data(0);
    int data1 = set1.get_data(0);
    if (clique)
        // TODO : Try more kinds of calculation.
        // For example, we can use binary search find the last element which is smaller than min_vertex, and set its index as loop_size.
        while (i < size0 && j < size1)
        {
            if (data0 < data1)
            {
                if ((data0 = set0.get_data(++i)) >= min_vertex)
                    break;
            }
            else if (data0 > data1)
            {
                if ((data1 = set1.get_data(++j)) >= min_vertex)
                    break;
            }
            else
            {
                push_back(data0);
                if ((data0 = set0.get_data(++i)) >= min_vertex)
                    break;
                if ((data1 = set1.get_data(++j)) >= min_vertex)
                    break;
            }
        }
    else
        while (i < size0 && j < size1)
        {
            data0 = set0.get_data(i);
            data1 = set1.get_data(j);
            if(set0.mask != nullptr && set0.mask->test(data0) == false)
                ++i;
            else if(set1.mask != nullptr && set1.mask->test(data1) == false)
                ++j;
            else if (data0 < data1)
                ++i;
            else if (data0 > data1)
                ++j;
            else
            {
                push_back(data0);
                ++i;
                ++j;
            }
        }
}

// ! aborted: not used
void VertexSet::intersection_with(const VertexSet& set1) {
    if (this == &set1)
        return;
    const VertexSet& set0 = *this;
    int i = 0;
    int j = 0;
    int size0 = set0.get_size();
    int size1 = set1.get_size();

    // TODO : Try more kinds of calculation.
    // Like
    // while (true)
    //     ..., if (++i == size0) break;
    //     ..., if (++j == size1) break;
    //     ......
    // Maybe we can also use binary search if one set is very small and another is large.
    int data0 = set0.get_data(0);
    int data1 = set1.get_data(0);
    size = 0;
    while (i < size0 && j < size1)
    {
        data0 = set0.get_data(i);
        data1 = set1.get_data(j);
        if (data0 < data1)
            ++i;
        else if (data0 > data1)
            ++j;
        else
        {
            push_back(data0);
            ++i;
            ++j;
        }
    }
}

void VertexSet::build_vertex_set(const Schedule& schedule, const VertexSet* vertex_set, int* input_data, int input_size, int prefix_id, int max_intersection_size, int min_vertex, bool clique, MaskPtr mask)
{
    int father_id = schedule.get_father_prefix_id(prefix_id);
    if (father_id == -1)
        init(input_size, input_data, mask); // the 1-st layer vertex set should have mask
    else
    {
        // we dont want the intersected set have mask
        init(max_intersection_size);
        VertexSet tmp_vset;
        tmp_vset.init(input_size, input_data, mask);
        intersection(vertex_set[father_id], tmp_vset, min_vertex, clique);
    }
}

// ! aborted: not used
void VertexSet::insert_ans_sort(int val)
{
    int i;
    for (i = size - 1; i >= 0; --i)
        if (data[i] >= val)
            data[i + 1] = data[i];
        else
        {
            data[i + 1] = val;
            break;
        }
    if (i == -1)
        data[0] = val;
    ++size;
}

bool VertexSet::has_data(int val)
{
    for (int i = 0; i < size; ++i)
        if (data[i] == val)
            return true;
    return false;
}

// ! not used
int VertexSet::unorderd_subtraction_size(const VertexSet& set0, const VertexSet& set1, int size_after_restrict)
{
    int size0 = set0.get_size();
    int size1 = set1.get_size();
    if (size_after_restrict != -1)
        size0 = size_after_restrict;

    int ret = size0;
    const int* set0_ptr = set0.get_data_ptr();
    for (int j = 0; j < size1; ++j)
        if (std::binary_search(set0_ptr, set0_ptr + size0, set1.get_data(j)))
            --ret;
    return ret;
}

// * DEBUG for embbedings listing task
int VertexSet::unorderd_subtraction_size_list(const VertexSet& set0, const VertexSet& set1, int isTest, std::vector<int>& ans, int size_after_restrict)
{
    int size0 = set0.get_size();
    int size1 = set1.get_size();
    if (size_after_restrict != -1)
        size0 = size_after_restrict;

    const int* set1_ptr = set1.get_data_ptr(); 
    std::unordered_set<int> set1_hash(set1_ptr, set1_ptr + size1);

    for (int i = 0; i < size0; ++i) {
        int data0 = set0.get_data(i);
        if(set0.mask != nullptr && set0.mask->test(data0) == false) continue;
        if (set1_hash.find(data0) == set1_hash.end()) {
            ans.push_back(data0);
        }
    }



    // int i=0, j=0;
    // while (i < size0 && j < size1) {
    //     if (set0.get_data(i) < set1.get_data(j)) {
    //         // 如果set0中的当前元素不存在于set1中，将其添加到ans中
    //         ans.push_back(set0.get_data(i));
    //         i++; // 移动set0的指针
    //     } else if (set0.get_data(i) > set1.get_data(j)) {
    //         j++; // 移动set1的指针
    //     } else {
    //         // 如果元素在set0和set1中都存在，同时移动两个指针
    //         i++;
    //         j++;
    //     }
    // }

    // // 处理set0中剩余的元素
    // while (i < size0) {
    //     ans.push_back(set0.get_data(i));
    //     i++;
    // }
    // if(isTest==0){
    //     // printf("ret %d\n",ret);

    //     // int _cnt=0;
        
    //     std::vector<int> v;
    //     for(int _j=0;_j<size0;_j++){
    //         v.clear();
    //         int inval=0;
    //         for(int _t=0;_t<cnt;_t++)
    //             if(set0.get_data(_j)==invalid[_t]){
    //                 inval=1;
    //                 break;
    //             }
    //         if(inval) continue;

    //         for(int _i=0;_i<size1;_i++){
    //             // printf("%d ", set1.get_data(_i));
    //             v.push_back(set1.get_data(_i));
    //         }
    //         // printf("%d\n",set0.get_data(_j));
    //         v.push_back(set0.get_data(_j));
    //         ans.push_back(v);
    //     }
    //     // printf("cnt %d ret %d,%d %d \n",_cnt,ret,size0,size1);
    // }

    return ans.size();
}
