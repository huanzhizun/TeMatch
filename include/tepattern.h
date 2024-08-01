#pragma once
#include <bits/stdint-uintn.h>
#include <string>
#include <assert.h>
#include <vector>
#include "schedule.h"

struct TePattern{
    uint32_t n;
    uint32_t e;

    uint32_t maxOut;
    uint32_t maxLevel;

    uint32_t *child;
    uint32_t *offset;
    uint32_t *fa;
    uint32_t *mode; // (from*n+to) -> edge mode
    uint32_t *edgeIndx; //(from*n+to) -> edge index //* ONLY used for output result
    uint32_t *level; //(from*n+to) -> edge depth level
    uint32_t *maxDepth; // node -> max node depth;
    uint32_t *inDgree;

    uint32_t *label;

    uint32_t *nodeMap;
    uint32_t originN;
    uint32_t root;

    std::vector<std::vector<uint32_t>> descendant;

    std::vector<std::vector<uint32_t>> in;


    // * USED ONLY FOR CPATTERN

    uint32_t **adjmat;
    uint32_t *edgeSchedule;
    uint32_t *pfrom;
    uint32_t *pto;


    TePattern(){
        n=e=maxOut=maxLevel=0;
        root=0;
        child=nullptr;
        offset=nullptr;
        fa=nullptr;
        mode=nullptr;
        edgeIndx=nullptr;
        level=nullptr;
        maxDepth=nullptr;
        inDgree=nullptr;
        label=nullptr;
        edgeSchedule=nullptr;
        adjmat=nullptr;
        pfrom=nullptr;
        pto=nullptr;
    };

    ~TePattern(){
        if(child){delete []child;child=nullptr;}
        if(fa){delete  []fa;fa=nullptr;}
        if(offset){delete []offset;offset=nullptr;}
        if(mode){delete []mode;mode=nullptr;}
        if(edgeIndx){delete []edgeIndx;edgeIndx=nullptr;}
        if(level){delete []level;level=nullptr;}
        if(maxDepth){delete []maxDepth;maxDepth=nullptr;}
        if(inDgree){delete []inDgree;inDgree=nullptr;}
        if(label){delete []label;label=nullptr;}
        for(int i=0;i<originN;i++){
            if(adjmat[i]){delete []adjmat[i];adjmat[i]=nullptr;}
        }
        if(adjmat){delete []adjmat;adjmat=nullptr;}
        if(edgeSchedule){delete []edgeSchedule;edgeSchedule=nullptr;}
        if(pfrom){delete []pfrom;pfrom=nullptr;}
        if(pto){delete []pto;pto=nullptr;}
        n=e=maxOut=maxLevel=0;
        root=0;
        originN=0;
    }

    bool loadPattern(const Schedule& schedule, bool pathTree = 0);

    int LoadData(std::string path);

    void loadLabel(std::string path);

    int dfsMode(uint32_t root, uint32_t &mode);

    int automorphismMode(uint32_t root);

    int automorphismCheck(uint32_t root, uint32_t depth);

    int ScheduleGenerate(std::vector<uint32_t> &schedule);

    void getPaths(std::vector<std::vector<int>> &paths, std::vector<int>& edges, uint32_t root) const;

    void ExtraPaths(std::vector<std::vector<int>> &extraPaths, const Schedule& schedule);

    int Descendant();

    int DescendantFunc(uint32_t node);

    void edgeScheduleGenerate(std::vector<uint32_t> &schedule);

    void edgeScheFunc(std::vector<uint32_t> &schedule, uint32_t node, char *vis);


    inline int getChild(uint32_t vid, uint32_t &l, uint32_t &r) const{
        assert(this->offset);
        assert(vid < n);
        l=this->offset[vid];
        r=this->offset[vid+1];
        return l<r;
    };

    inline uint32_t modeId(uint32_t from, uint32_t to) const{
        uint64_t t=from*n+to;
        assert(~(t>>32)); // overflow check;
        return (uint32_t)t;
    }

    inline uint32_t outputId(uint32_t from, uint32_t to) const{
        return this->modeId(from,to);
    }

    inline uint32_t levelId(uint32_t from, uint32_t to) const{
        return this->modeId(from,to);
    }

};