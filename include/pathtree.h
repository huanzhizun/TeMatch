#pragma once
#include "stdint.h"

#include <vector>

class Schedule;
class TePattern;
class TeGraph;


class PathTree{
public:

    int n;
    int e;

    int *mode;
    int *offset;
    int *child;
    int *nodeMap;    // new label -> old label

    std::vector<std::vector<int>> paths;

    PathTree(){
        n = 0;
        e = 0;
        mode = nullptr;
        offset = nullptr;
        child = nullptr;
        nodeMap = nullptr;
    }

    PathTree(const Schedule& schedule, const TePattern& tp);

    ~PathTree(){
        if(mode) {delete []mode; mode=nullptr;}
        if(offset) {delete []offset; offset=nullptr;}
        if(child) {delete []child; child=nullptr;}
        if(nodeMap) {delete []nodeMap; nodeMap=nullptr;}
        n=e=0;
    }

    inline int modeId(int from, int to) const{
        return from*n+to;
    }

    bool validate(const std::vector<int>& embed, const TeGraph& tg, int tgroot, int ptroot);

    

};