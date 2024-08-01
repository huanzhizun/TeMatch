#pragma once
#include "schedule.h"
class Graph;
#define MAX_MASK_SIZE (1094018 + 200)
#include <bitset>
#include <memory>

using MaskPtr = std::shared_ptr<const std::bitset<MAX_MASK_SIZE>>;
class VertexSet
{
public:
    VertexSet();
    // allocate new memory according to max_intersection_size
    void init(const Graph& g, MaskPtr mask=nullptr);
    void init(int init_size, MaskPtr mask=nullptr);
    // use memory from Graph, do not allocate new memory
    void init(int input_size, int* input_data, MaskPtr mask=nullptr);
    void copy(int input_size, const int* input_data, MaskPtr mask=nullptr);
    ~VertexSet();
    void intersection(const VertexSet& set0, const VertexSet& set1, int min_vertex = -1, bool clique = false);
    void intersection_with(const VertexSet& set1);
    //set1 is unordered
    static int unorderd_subtraction_size(const VertexSet& set0, const VertexSet& set1, int size_after_restrict = -1);
    static int unorderd_subtraction_size_list(const VertexSet& set0, const VertexSet& set1,int isTest, std::vector<int>& ans, int size_after_restrict=-1);
    void insert_ans_sort(int val);
    inline int get_size() const { return size;}
    inline int get_data(int i) const { return data[i];}
    inline const int* get_data_ptr() const { return data;}
    inline int* get_data_ptr() { return data;}
    inline void push_back(int val) { data[size++] = val;}
    inline void pop_back() { --size;}
    inline int get_last() const { return data[size - 1];}
    bool has_data(int val);

    void build_vertex_set(const Schedule& schedule, const VertexSet* vertex_set, int* input_data, int input_size, int prefix_id, int max_intersection_size, int min_vertex = -1, bool clique = false, MaskPtr mask = nullptr);

    MaskPtr mask = nullptr;
private:
    int* data;
    int size;
    bool allocate;
};
