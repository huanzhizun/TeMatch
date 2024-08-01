#pragma once


#include <bitset>
#include <bits/types.h>
#include <string>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <iostream>
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include <bitset>
#include <map>

#include "common.h"
#include "tepattern.h"
#include "config.h"

#define INVALID ((uint32_t)-1)

struct _stat{
    double time=0;
};


enum MatchType{
    NEW_MATCH_LEAF = 0,
    NEW_MATCH_INTER,
    MATCHED_BEFORE,
    MATCH_FAIL
};

struct MatchState{
    MatchType t;
    uint32_t vid;  //used only for NEW_MATCH_INNER or MATCHED_BEFORE type
};

static absl::flat_hash_set<uint64_t> defaultsets;

extern struct _stat compact_stat;

struct TeGraph{
    // after transformed count
    uint32_t tnodeCnt;
    uint32_t tedgeCnt;

    // original count 
    uint32_t nodeCnt;
    uint32_t edgeCnt;

    // vid 是连续的单调递增序列，因为分配时是从小的(node<<32)|time开始，同nid下，vid小则t小
    uint32_t *edge=nullptr;          // offset -> vid
    uint32_t *offset=nullptr;        // vid -> edgeoffset
    // absl::flat_hash_map<uint64_t,uint32_t> nodeMap;   // map<info, vid> info:(uint64_t)(id)<<32|time
    std::unordered_map<uint64_t, uint32_t> nodeMap;
    uint64_t *nodeInfo=nullptr;       // vid -> info : (uint64_t)(id)<<32|time
    uint32_t *timeOffset=nullptr;     // nodeid -> vid
    uint32_t *outNode=nullptr;        // list of node whose out dgree != 0
    uint32_t outNodeCnt=0;
    uint32_t *outDgree=nullptr;
    uint32_t *inDgree=nullptr;
    uint32_t *label=nullptr;       // nid -> label
    uint32_t maxOut=0;        // max out dgree
    uint32_t maxStaticOut=0;  // max out dgree in static Te

    //maxPattern is depend on query(pattern) size
    uint32_t maxPattern;
    uint8_t *vis; //vid*maxPattern+pattern -> bool
    uint32_t *candidateNum; //vid*maxPattern+pattern -> int
    uint32_t *candidate; //[vid*maxPattern*maxOut+pattern*maxOut+candidateNum] -> vid
    uint32_t *child; //vid*maxPattern+pattern -> vid
    uint32_t *next;  //vid*maxPattern+pattern -> vid
    uint32_t *prev;  //vid*maxPattern+pattern -> vid //* the prev and next form a bidirctional list

    uint32_t *last; //nid*maxPattern+pattern -> vid  //* last access on a specific mode 
    uint32_t *patternRecord; // nid*maxPattern + pattern count -> matched pattern
    uint32_t *patternNum;  //nid -> matched pattern num;
    uint32_t *timecandidateNum; //vid*maxPattern+pattern -> int 包括vid及同nid下，比vid时间大的所有节点的candidateNum
    uint32_t *firstMatched;  // modeId -> first matched vid

    std::unordered_set<uint64_t> staticEdges;

    std::bitset<BITSET_SIZE> *globalReach;

    // std::unordered_map<uint64_t, std::map<uint32_t, uint32_t>> minimalArrive;
    // minimalArrive[u][v][s]
    std::unordered_map<uint32_t, std::unordered_map<uint32_t, std::map<uint32_t, uint32_t>>> minimalArrive;

    // // for store
    // State* state;

    TeGraph(){
        tnodeCnt=tedgeCnt=outNodeCnt=maxOut=maxStaticOut=0;
        nodeCnt=edgeCnt=0;
        maxPattern=0;
        // unitAlloc=totPool=0;
        offset=nullptr;
        edge=nullptr;
        nodeInfo=nullptr;
        timeOffset=nullptr;
        outNode=nullptr;
        outDgree=nullptr;
        inDgree=nullptr;
        vis=nullptr;
        candidateNum=nullptr;
        candidate=nullptr;
        child=nullptr;
        next=nullptr;
        prev=nullptr;
        last=nullptr;
        patternRecord=nullptr;
        patternNum=nullptr;
        timecandidateNum=nullptr;
        firstMatched=nullptr;
        globalReach=nullptr;
        label=nullptr;
    };

    ~TeGraph() {this->reset();};

    // [l,r) 
    // be careful that if offset[i] == 0 , then offset[i]-1 would be 4294967295 !!
    inline int getNeighbor(uint32_t vid, uint32_t &l, uint32_t &r){
        assert(this->offset);
        // if(vid>=tnodeCnt) printf("%d\n", vid);
        assert(vid < this->tnodeCnt);
        l=this->offset[vid];
        r=this->offset[vid+1];
        return l<r;
    };

    // [tl,tr) 
    inline int getTimeList(uint32_t nid, uint32_t &tl, uint32_t &tr){
        assert(this->timeOffset);
        // assert(nid < this->nodeCnt);
        tl=this->timeOffset[nid];
        tr=this->timeOffset[nid+1];
        return tl<tr;
    };

    inline int getInfo(uint32_t vid, uint32_t &nid, uint32_t &time){
        assert(vid<this->tnodeCnt);
        nid=this->nodeInfo[vid]>>32;
        time=(this->nodeInfo[vid]&0xffffffff);
        return 1;
    };

    inline uint32_t storeId(uint32_t vid, uint32_t TePattern) const{
        uint64_t t=vid*maxPattern+TePattern;
        assert(~(t>>32)); // overflow check;
        return (uint32_t)t;
    }

    inline uint32_t candidateId(uint32_t vid, uint32_t TePattern, uint32_t can) const{
        uint64_t t=vid*maxPattern*maxOut+TePattern*maxOut+can;
        assert(~(t>>32));
        return (uint32_t)t;
    }

    inline uint32_t lastId(uint32_t nid, uint32_t TePattern){
        uint64_t t=nid*maxPattern+TePattern;
        assert(~(t>>32));
        return (uint32_t)t;
    }

    inline uint32_t patternRecordId(uint32_t nid, uint32_t patternCount){
        uint64_t t=nid*patternCount+patternCount;
        assert(~(t>>32));
        return (uint32_t)t;
    }
    
    inline uint32_t time(uint32_t vid) const{
        if(vid>=tnodeCnt) printf("%u\n",vid);
        assert(vid<tnodeCnt);
        return this->nodeInfo[vid]&(0xffffffff);
    }

    inline uint32_t nid(uint32_t vid) const{
        assert(vid<tnodeCnt);
        return this->nodeInfo[vid]>>32;
    }

    int LoadData(std::string path);

    int match(TePattern* p, const Graph* g, std::vector<uint32_t> &ans);

    int match_func(TePattern* p, uint32_t vfrom, uint32_t vto, uint32_t qlastmode, uint32_t qto, MatchState &s, const Graph* g);

    int preBuilt(TePattern* p);

    int reset();

    void StorageGetIntersec(const TePattern& p, uint32_t pentry, const std::vector<uint32_t>& candidateVid,std::vector<std::bitset<BITSET_SIZE>> &intersection);

    void EdgeCompact(const TePattern& p, uint32_t pentry, const std::vector<uint32_t>& candidateVid, const std::vector<std::bitset<BITSET_SIZE>> &intersection, absl::flat_hash_set<uint64_t> &usedEdge, std::vector<std::pair<uint32_t, uint32_t>> &edges, absl::flat_hash_map<uint64_t, std::vector<uint64_t>>* time = nullptr);


    void PrevBuild(TePattern *p, uint32_t pentry);

    void GetLSG(const TePattern& p, uint32_t gentry);

};