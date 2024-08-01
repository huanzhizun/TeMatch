#include "pathtree.h"
#include "schedule.h"
#include "tepattern.h"
#include "tegraph.h"

PathTree::PathTree(const Schedule& schedule, const TePattern& tp){
    printf("path tree loading\n");
    // e = schedule.edges.size();

    // std::vector<std::vector<int>> tpaths;
    // std::vector<int> edges;

    // edges.push_back(0);
    // tp.getPaths(tpaths, edges, 0);

    this->paths = schedule.paths;

    // for(auto extra: paths_temp){
    //     int inTPaths=0;
    //     for(auto p: tpaths){

    //         if(extra.size() != p.size()) continue;
    //         int match = 1;
            
    //         for(int i=0;i<extra.size();i++){
    //             if(extra[i] != p[i]){
    //                 match=0;
    //                 break;
    //             }
    //         }

    //         if(match){
    //             inTPaths = 1;
    //             break;
    //         }
    //     }
    //     if(inTPaths == 0) this->paths.push_back(extra);
    // }

    // printf("paths that TCS can not confirm : \n");
    // for(auto p: this->paths){
    //     for(auto e: p) printf("%d ", e);
    //     printf("\n");
    // }

    int nodeCnt = 0;
    std::unordered_set<uint64_t> edgeSet;
    std::unordered_map<int, int> map;
    std::vector<std::pair<int, int>> treeEdge;

    map.clear();
    edgeSet.clear();
    map[nodeCnt++] = 0;
    
    for(auto p: this->paths){
        std::vector<int> tem;
        tem.clear();
        
        for(int i=1; i<p.size(); i++){
            int f;
            if(i==1){
                f = p[i-1];
                tem.push_back(f);
            }
            else{
                f = tem[tem.size()-1];
            }

            int c = p[i];
            

            uint64_t temp = (((uint64_t)f)<<32)|c;
            if(edgeSet.find(temp) == edgeSet.end()){
                edgeSet.insert(temp);
                treeEdge.push_back(std::make_pair(f, nodeCnt));
                map[nodeCnt] = c;
                tem.push_back(nodeCnt);
                nodeCnt++;
            }
            else{
                tem.push_back(c);
            }
        }
    }

    nodeMap = new int[nodeCnt];
    memset(nodeMap, -1, sizeof(int)*nodeCnt);

    printf("path tree nodeMap: \n");
    for(int i=0;i<nodeCnt;i++){
        nodeMap[i] = map[i];
        printf("%d : %d\n", i, nodeMap[i]);
    }

    std::sort(treeEdge.begin(), treeEdge.end());


    n = nodeCnt;

    mode = new int[n*n+1];
    memset(mode, -1, sizeof(int)*(n*n+1));

    // * Pattern Tree -> 原图
    int _temMode[schedule.size*schedule.size+1];
    memset(_temMode, -1, sizeof(int)*(schedule.size*schedule.size+1));

    for(int i=0;i<tp.n;i++)
        for(int j=0;j<tp.n;j++)
            if(tp.mode[tp.modeId(i,j)] != -1) 
                _temMode[tp.nodeMap[i]*schedule.size + tp.nodeMap[j]] = tp.mode[tp.modeId(i,j)];


    // * 原图 -> Path Tree
    printf("tree: \n");
    for(auto e: treeEdge){
        printf("%d -> %d\n", e.first, e.second);

        //! 处理mode信息：
        mode[modeId(e.first,e.second)] = _temMode[nodeMap[e.first]*schedule.size+nodeMap[e.second]];

    }

    printf("Path Tree mode check:\n");
    for(int i=0;i<n;i++)
        for(int j=0;j<n;j++)
            if(mode[modeId(i,j)]!=-1) printf("(%d,%d): %d\n", i, j, mode[modeId(i,j)]);    

}

bool PathTree::validate(const std::vector<int>& embed, const TeGraph& tg, int tgroot, int ptroot){

    std::vector<int> pChilds;
    pChilds.clear();

    int findFlag[this->n];
    memset(findFlag, 0, sizeof(int)*this->n);

    for(int nxtp = 0; nxtp < this->n; nxtp++){
        int mode = this->mode[this->modeId(ptroot, nxtp)];
        int find = 0;

        if(mode != -1){
            pChilds.push_back(nxtp);
            uint32_t tvid = (uint32_t)tgroot;
            uint32_t nxtEmbed = (uint32_t)embed[nodeMap[nxtp]];
            while (tvid != INVALID)
            {
                uint32_t strid = tg.storeId(tvid, mode);

                if(tg.vis[strid] && tg.candidateNum[strid] !=0){

                    uint32_t canNum = tg.candidateNum[strid];
                    for(int idx=0;idx<canNum;idx++){
                        uint32_t canId = tg.candidateId(tvid, mode, idx);
                        uint32_t nxtvid = tg.candidate[canId];

                        if(tg.nid(nxtvid) != nxtEmbed) continue;

                        int ret = validate(embed, tg, nxtvid, nxtp);

                        if(ret){
                            find = 1;
                            break;
                        }
                    }
                }

                if(find) break;

                tvid = tg.next[strid];
            }

            findFlag[nxtp] = find;
            if(find==0) break;

        }
    }

    int f = 1;
    for(auto e: pChilds){
        if(findFlag[e] == 0){
            f=0;
            break;
        }
    }

    return f?true:false;

}