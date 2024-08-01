#include "tegraph.h"
#include <fstream>
#include <assert.h>
#include <string>
#include "graph.h"
#include <bitset>
#include <queue>

// #define STORE_DEBUG


// #define assert(...) do{}while(0)

#ifdef DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...) do{}while(0)
#endif


std::string MatchTypeStr[]={
    "NEW_MATCH_LEAF",
    "NEW_MATCH_INTER",
    "MATCHED_BEFORE",
    "MATCH_FAIL"
};


/*
*  dataformate: 
*     node.data:  id, time
*     edge.data:  from_id, to_id, from_time, to_time
*/
int TeGraph::LoadData(std::string path){
    // ! 输入数据必须是原始节点序和时间序组合的升序
    printf("hint: the edge.data and node.data need to be sorted in ascending order on start nid and start time\n");

    this->reset();

    std::string pn=path+"node.data";
    std::string pe=path+"edge.data";
    // ifstream data(path, ios::in|ios::binary);
    FILE* fpe = fopen(pe.c_str(), "rb");
    FILE* fpn=fopen(pn.c_str(),"rb");
    assert(fpe);
    assert(fpn);

    // cout<<"asdasd"<<'\n';
    int f=fread((char*)&tnodeCnt, sizeof(uint32_t), 1, fpn);
    f=fread((char*)&tedgeCnt, sizeof(uint32_t), 1, fpe);
    printf("edge : %d node : %d\n", tedgeCnt,tnodeCnt);

    uint32_t *bufn = new uint32_t[tnodeCnt*2];
    uint32_t *bufe = new uint32_t[tedgeCnt*4];
    edge = new uint32_t[tedgeCnt];
    nodeInfo = new uint64_t[tnodeCnt];
    offset = new uint32_t[tnodeCnt+1];
    memset(offset, -1 ,(tnodeCnt+1)*sizeof(uint32_t));
    timeOffset = new uint32_t[tnodeCnt+1];  //TODO: no need to use tnodeCnt, just need be origin vertex count + 1
    memset(timeOffset, -1 ,(tnodeCnt+1)*sizeof(uint32_t));
    outNode = new uint32_t[tedgeCnt+1];
    outDgree=new uint32_t[tnodeCnt+1];
    memset(outDgree,0,sizeof(uint32_t)*(tnodeCnt+1));
    inDgree=new uint32_t[tnodeCnt+1];
    memset(inDgree,0,sizeof(uint32_t)*(tnodeCnt+1));
    // cout<<"asdasd"<<'\n';
    
    
    // uint32_t *node = new uint32_t[edge*4];

    f=fread((char*)bufn, sizeof(uint32_t), tnodeCnt*2, fpn);
    f=fread((char*)bufe, sizeof(uint32_t), tedgeCnt*4, fpe);

    fclose(fpn);
    fclose(fpe);

    // cout<<"asdasd"<<'\n';
    auto t = get_wall_time();
    uint32_t idt = bufn[0];
    timeOffset[idt]=0;
    nodeCnt++;
    for(uint32_t i=0;i<tnodeCnt;i++){
        uint64_t t=((uint64_t)bufn[2*i]<<32|bufn[2*i+1]);
        // if(t<65535) printf("%ld %d %d\n", t, bufn[2*i], bufn[2*i+1]);
        nodeMap[t] = i;
        nodeInfo[i]=t;
        if(bufn[2*i]!=idt){
            idt=bufn[2*i];
            timeOffset[idt]=i;
            nodeCnt++;
        }
    }
    timeOffset[idt+1]=tnodeCnt;
    for(int i=idt;i>=0;--i){
        if(timeOffset[i]==INVALID) timeOffset[i]=timeOffset[i+1];
    }
    std::cout<<"loading vertex costs: "<<get_wall_time()-t<<" s\n";
    for(int i=idt;i>=0;--i){
        if(timeOffset[i]==INVALID || timeOffset[i]>timeOffset[i+1])
            printf("%d %d %d\n",i,timeOffset[i],timeOffset[i+1]);
        assert(timeOffset[i]!=INVALID && timeOffset[i]<=timeOffset[i+1]);
    }
    // cout<<"asdasd"<<'\n';
    delete []bufn;
    bufn=nullptr;
    t = get_wall_time();
    uint64_t p=((uint64_t)bufe[0]<<32|bufe[2]);
    offset[nodeMap[p]]=0;
    outNode[0]=nodeMap[p];
    uint64_t tempDgree=0;
    uint32_t lastNid=bufe[0];
    for(uint32_t i=0;i<tedgeCnt;i++){
        uint64_t from = ((uint64_t)bufe[4*i]<<32|bufe[4*i+2]);
        uint64_t to=((uint64_t)bufe[4*i+1]<<32|bufe[4*i+3]);
        uint32_t vto=nodeMap[to];
        uint32_t vform=nodeMap[from];

        uint64_t __e = ((uint64_t)bufe[4*i]<<32|bufe[4*i+1]);
        staticEdges.insert(__e);
     
        // uint64_t e=((uint64_t)bufe[4*i]<<32|bufe[4*i+1]);
        // if(minimalArrive.find(e) == minimalArrive.end()){
        //     minimalArrive[e] = std::unordered_map<uint32_t, uint32_t>();
        // }

        edge[i]=vto;
        inDgree[vto]++;
        outDgree[vform]++;
        tempDgree++;
        
        // printf("%d\n", nodeMap[from]);

        if(from!=p){
            p = from;
            offset[vform]=i;
            outNode[++outNodeCnt]=vform;
        }
        if(lastNid != nid(vform)){
            maxStaticOut = maxStaticOut<tempDgree?tempDgree:maxStaticOut;
            tempDgree=0;
            lastNid = nid(vform);
        }
    }
    outNodeCnt++;
    assert(outNodeCnt<=tedgeCnt);
    offset[tnodeCnt]=tedgeCnt;
    for(int i=tnodeCnt-1;i>=0;--i){
        if(offset[i]==INVALID) offset[i]=offset[i+1];
        if(outDgree[i]>maxOut) maxOut=outDgree[i];
    }
    std::cout<<"loading edge costs: "<<get_wall_time() - t<<" s\n";
    delete []bufe;
    bufe=nullptr;
    for(int i=tnodeCnt-1;i>=0;--i){
        assert(offset[i]!=INVALID && offset[i]<=offset[i+1]);
    }
    printf("outNode : %d, maxOut: %d, maxStaticOut: %d\n", outNodeCnt, maxOut, maxStaticOut);

    return 1;
}

int TeGraph::preBuilt(TePattern* p){
    assert(p->e);
    assert(maxOut);
    assert(tnodeCnt);
    uint64_t t1=tnodeCnt*maxPattern;
    uint64_t t2=tnodeCnt*maxPattern*maxOut;
    assert(~(t1>>32)); // overflow check
    assert(~(t2>>32));
    // assert(BITSET_SIZE > nodeCnt);

    maxPattern=0;
    for(uint32_t i=0;i<p->n;i++)
        for(uint32_t j=0;j<p->n;j++){
            uint32_t mode=p->mode[p->modeId(i,j)];
            if(mode!=INVALID) maxPattern=(maxPattern<mode?mode:maxPattern);
        }
    
    maxPattern++;
    
    vis=new uint8_t[tnodeCnt*maxPattern+1];
    memset(vis,0,sizeof(uint8_t)*(tnodeCnt*maxPattern+1));
    candidateNum=new uint32_t[tnodeCnt*maxPattern+1];
    memset(candidateNum,0,sizeof(uint32_t)*(tnodeCnt*maxPattern+1));
    // child=new uint32_t[tnodeCnt*maxPattern+1];
    // memset(child,INVALID,sizeof(uint32_t)*(tnodeCnt*maxPattern+1));
    next=new uint32_t[tnodeCnt*maxPattern+1];
    memset(next,INVALID,sizeof(uint32_t)*(tnodeCnt*maxPattern+1));
    // prev=new uint32_t[tnodeCnt*maxPattern+1];
    // memset(prev,INVALID,sizeof(uint32_t)*(tnodeCnt*maxPattern+1));
    candidate=new uint32_t[tnodeCnt*maxPattern*maxOut+1];
    memset(candidate,INVALID,sizeof(uint32_t)*(tnodeCnt*maxPattern*maxOut+1));
    last=new uint32_t[tnodeCnt*maxPattern+1];  // TODO:use max nid rather vid to allocate
    memset(last,INVALID,sizeof(uint32_t)*(tnodeCnt*maxPattern+1));
    // patternNum = new uint32_t[tnodeCnt + 1]; // TODO: use max nid rather vid to allocate
    // memset(patternNum, 0, sizeof(uint32_t)*(tnodeCnt + 1));
    // patternRecord = new uint32_t[tnodeCnt*maxPattern+1];
    // timecandidateNum = new uint32_t[tnodeCnt*maxPattern+1];
    // memset(timecandidateNum,0,sizeof(uint32_t)*(tnodeCnt*maxPattern+1));

    // firstMatched = new uint32_t[maxPattern+1];
    // memset(firstMatched, INVALID, sizeof(uint32_t)*(maxPattern+1));

    // globalReach = new std::bitset<BITSET_SIZE>[tnodeCnt+1];
    // for(int i=0;i<tnodeCnt+1;i++) globalReach[i].reset();

    return 1;
}

int TeGraph::match(TePattern* p, const Graph* g, std::vector<uint32_t> &ans){
    preBuilt(p);

    uint32_t qen=0;
    uint32_t ql,qr;
    p->getChild(qen,ql,qr);

    MatchState s={MATCH_FAIL, INVALID};
    uint32_t ansCount=0;
    
    
    // for(uint32_t iout=outNodeCnt-1;iout>=0&&iout!=INVALID;iout--){
    for(uint32_t iout=0;iout<outNodeCnt;iout++){
        uint32_t gen=outNode[iout];
        int ret = match_func(p,0,gen,0,qen,s,g);
        if(ret){
            ansCount++;
            // printf("%u\n",ansCount);
            // printf("[%d]:%u\n",ansCount,gen);
            ans.push_back(gen);
        }
        // printf("(%d,%d)\n",nid(gen),time(gen));
    }


    return ansCount;
}

int TeGraph::match_func(TePattern* p, uint32_t gfrom, uint32_t gto, uint32_t qfrom, uint32_t qto, MatchState &s, const Graph* g){

    uint32_t gtoid,gtotime,gtol,gtor;
    uint32_t qtoid,qtotime,qtol,qtor;
    getInfo(gto,gtoid,gtotime);

    uint32_t originN = p->nodeMap[qto];

    if(!p->getChild(qto,qtol,qtor)){
        
            s.t=NEW_MATCH_LEAF;
            s.vid=INVALID;
            return 1;
    }

    uint32_t gtotimel,gtotimer;
    getTimeList(gtoid,gtotimel,gtotimer);
    if(!getNeighbor(gto,gtol,gtor) && gto==(gtotimer-1)){
        s.t=MATCH_FAIL;
        s.vid=INVALID;
        return 0;
    }

    unsigned int gl, gr;
    g->get_edge_index(gtoid, gl, gr);
    if((qtor-qtol) > (gr-gl)){
        s.t=MATCH_FAIL;
        s.vid=INVALID;
        return 0;
    }

    for(uint32_t iq=qtol;iq<qtor;iq++){
        uint32_t qnxt=p->child[iq];
        uint32_t gnlast=INVALID;
        uint32_t qnxtmode=p->mode[p->modeId(qto,qnxt)];
        assert(qnxtmode!=INVALID);

        uint32_t stridlast=INVALID;
        if(gnlast!=INVALID) stridlast=storeId(gnlast,qnxtmode);

        uint32_t gstart=last[lastId(gtoid,qnxtmode)];
        if(gstart==INVALID) gstart=gtotimer-1;

        for(uint32_t gnode=gstart;gnode>=gto/*&&(int)gnode>=0*/;gnode--){
            if(gnode==INVALID){
                if(gto==0) break;
                else assert(0);
            }
            assert(gnode!=INVALID);
            assert(gnode<tnodeCnt);
            int find=0;
            uint32_t strid=storeId(gnode,qnxtmode);
            if(vis[strid]==0){
                uint32_t gnodel,gnoder;
                getNeighbor(gnode,gnodel,gnoder);
                for(uint32_t ig=gnodel;ig<gnoder;ig++){
                    uint32_t gnxt=edge[ig];
                    assert(time(gnode)<=time(gnxt));
                    int ret=match_func(p,gnode,gnxt,qto,qnxt,s,g);
                    // uint32_t pgi1,pgt1,pgi2,pgt2;
                    // getInfo(gnode,pgi1,pgt1);
                    // getInfo(gnxt,pgi2,pgt2);
                    // DEBUG_PRINT("ask(%u,%u), return from (%u,%u) with state %s on edge %u\n",
                    //     pgi1,pgt1,pgi2,pgt2,MatchTypeStr[s.t].c_str(),qnxtmode
                    // );
                    if(ret==1){
                        find=1;
                        uint32_t canNum=candidateNum[strid];
                        uint32_t canid=candidateId(gnode,qnxtmode,canNum);
                        candidateNum[strid]++;
                        candidate[canid]=gnxt;
                        
                        uint32_t uid, utime;
                        uint32_t vid, vtime;
                        getInfo(gnode, uid, utime);
                        getInfo(gnxt, vid, vtime);

                        if(minimalArrive.find(uid) == minimalArrive.end()) 
                            minimalArrive[uid] = std::unordered_map<uint32_t, std::map<uint32_t, uint32_t>>();
                        if(minimalArrive[uid].find(vid) == minimalArrive[uid].end())
                            minimalArrive[uid][vid] = std::map<uint32_t, uint32_t>();
                        if(minimalArrive[uid][vid].find(utime) == minimalArrive[uid][vid].end())
                            minimalArrive[uid][vid][utime] = vtime;
                        else{
                            uint32_t tmin = minimalArrive[uid][vid][utime];
                            minimalArrive[uid][vid][utime] = std::min(tmin, vtime);
                        }

                        // printf("%d -> %d, start %d minimal arrive %d\n",uid, vid, utime, minimalArrive[uid][vid][utime]);
                        
                        // if(s.t!=NEW_MATCH_LEAF) child[strid]=storeId(s.vid,qnxtmode);
                    }
                }

                if(find==0) candidateNum[strid]=0;
                if(gnode != (gtotimer-1)){
                    // printf("aaaa\n");
                    assert(stridlast!=INVALID);
                    if(vis[stridlast] && candidateNum[stridlast]!=0) {
                        assert((int)stridlast>=0);
                        next[strid]=gnlast;
                    }
                    else{
                        if((int)stridlast<0){
                            printf("%u %u %d %u %u",gnode,gtotimer-1,gtotimer-1,gstart,gto);
                            printf("%d %u\n",stridlast,stridlast);
                        }
                        assert((int)stridlast>=0);
                        next[strid]=next[stridlast];
                    }

                    if(next[strid] != INVALID){
                        uint32_t _gnxt = next[strid];
                        uint32_t uid, utime;
                        getInfo(gnode, uid, utime);
                        uint32_t uxttime = time(_gnxt);

                        uint32_t canNum=candidateNum[strid];
                        
                        for(int canCount=0; canCount < canNum; canCount++){
                            uint32_t canid=candidateId(gnode,qnxtmode,canCount);
                            // printf("%d\n", canNum);
                            uint32_t vid = nid(candidate[canid]);

                            if(minimalArrive[uid][vid].find(uxttime) == minimalArrive[uid][vid].end())
                                continue;

                            if(minimalArrive[uid][vid].find(utime) == minimalArrive[uid][vid].end())
                                minimalArrive[uid][vid][utime] = minimalArrive[uid][vid][uxttime];
                            else{
                                uint32_t tmin = minimalArrive[uid][vid][utime];
                                minimalArrive[uid][vid][utime] = std::min(tmin, minimalArrive[uid][vid][uxttime]);
                            }
                            // printf("[next]: %d -> %d, start %d minimal arrive %d\n",uid, vid, utime, minimalArrive[uid][vid][utime]);
                        }
                    }
                }
                else{
                    next[strid]=INVALID;
                }
            }
            else{
                DEBUG_PRINT("skipping (%d,%d), gstart (%d,%d), qnxtmode:%d\n",nid(gnode),time(gnode),nid(gstart),time(gstart),qnxtmode);
            }

            vis[strid]=1;
            gnlast=gnode;
            stridlast=storeId(gnlast,qnxtmode);
            if((int)stridlast<0) printf("%d %u\n",stridlast,stridlast);
            assert((int)stridlast>=0);
        }
        if(gto<=gstart) last[lastId(gtoid,qnxtmode)]=gto;

    }

    int matched=1;
    for(uint32_t iq=qtol;iq<qtor;iq++){
        uint32_t iqnxt=p->child[iq];
        uint32_t imode=p->mode[p->modeId(qto,iqnxt)];
        uint32_t istrid=storeId(gto,imode);
        if(candidateNum[istrid]==0&&next[istrid]==INVALID){
            matched=0;
            break;
        }
    }

    if(matched){
        s.t=NEW_MATCH_INTER;
        s.vid=gto;
    }
    else{
        s.t=MATCH_FAIL;
        s.vid=INVALID;
    }

    return (matched?1:0);
}

int TeGraph::reset(){
    if(edge){ delete[] edge;edge=nullptr;}
    if(offset) {delete[] offset;offset=nullptr;}
    if(nodeInfo) {delete[] nodeInfo;nodeInfo=nullptr;}
    if(timeOffset) {delete[] timeOffset;timeOffset=nullptr;}
    if(outNode) {delete[] outNode;outNode=nullptr;}
    if(outDgree) {delete[] outDgree;outDgree=nullptr;}
    if(inDgree) {delete[] inDgree;inDgree=nullptr;}
    if(vis) {delete[] vis;vis=nullptr;}
    if(candidateNum) {delete[] candidateNum;candidateNum=nullptr;}
    if(candidate) {delete[] candidate;candidate=nullptr;}
    if(child) {delete[] child;child=nullptr;}
    if(next) {delete[] next;next=nullptr;}
    if(prev) {delete[] prev;prev=nullptr;}
    if(last) {delete[] last;last=nullptr;}
    if(patternNum) {delete[] patternNum;patternNum=nullptr;}
    if(patternRecord) {delete[] patternRecord;patternRecord=nullptr;}
    if(timecandidateNum) {delete[] timecandidateNum;timecandidateNum=nullptr;}
    if(firstMatched) {delete[] firstMatched;firstMatched=nullptr;}
    if(globalReach) {delete[] globalReach;globalReach=nullptr;}
    // if(pool){
    //     for(int i=0;i<totPool;i++){
    //         if(pool[i]) {delete[] pool[i];pool[i]=nullptr;}
    //     }
    //     delete[] pool;
    // }
    // if(poolCnt) {delete[] poolCnt;poolCnt=nullptr;}
    // if(poolSize) {delete[] poolSize;poolSize=nullptr;}
    // if(state) {delete[] state;state=nullptr;}
    
    nodeMap.clear();
    tnodeCnt=tedgeCnt=outNodeCnt=maxOut=maxStaticOut=0;
    nodeCnt=edgeCnt=0;
    maxPattern=0;
    // unitAlloc=totPool=0;
    return 1;
}

// void TeGraph::Check(std::string outPath){

//     // printf("%u\n",outNode[outNodeCnt-1]);

//     std::string path=outPath+"edge.check";
//     FILE* f=freopen(path.c_str(), "w", stdout);
//     assert(f);
//     for(int i=0;i<outNodeCnt;i++){
//         uint32_t vfrom = outNode[i];
//         uint32_t l,r;
//         getNeighbor(vfrom, l ,r);
//         for(int j=l;j<r;j++){
//             uint32_t vto=edge[j];
//             uint32_t totime,toid,fromtime,fromid;
//             getInfo(vfrom,fromid,fromtime);
//             getInfo(vto,toid,totime);
//             printf("%u %u\n", vfrom, vto);
//         }
//         // if(i==outNodeCnt-1){
//         //     printf("%u %u\n", vfrom, edge[r]);
//         // }
//     }
//     fclose(f);


//     path=outPath+"node.check";
//     f=freopen(path.c_str(), "w", stdout);
//     assert(f);
//     printf("%u\n",tnodeCnt);
//     for(int i=0;i<tnodeCnt;i++){
//         uint32_t t;
//         printf("%u %u %u\n",i, nid(i), time(i));
//     }
//     fclose(f);
// }


void TeGraph::StorageGetIntersec(const TePattern& p, uint32_t pentry, const std::vector<uint32_t>& candidateVid,     std::vector<std::bitset<BITSET_SIZE>> &intersection){

    std::bitset<BITSET_SIZE> tmp;
    tmp.reset();
    uint32_t originN = p.nodeMap[pentry];
    for(auto can: candidateVid) tmp.set(nid(can));

    // printf("intersec [%d], origin [%d]: ", pentry, originN);
    // for(int j=nodeCnt;j>=0;j--) std::cout<<intersection[originN][j];
    // printf("\n");
    // printf("tmp [%d], origin [%d]: ", pentry, originN);
    // for(int j=nodeCnt;j>=0;j--) std::cout<<tmp[j];
    // printf("\n");

    intersection[originN] &= tmp;
    // printf("[%d]: %d %d %d\n", pentry, tmp.size(), tmp.count(), intersection[originN].count());

    uint32_t cl,cr;
    p.getChild(pentry, cl,cr);
    for(uint32_t chid = cl; chid<cr; chid++){
        std::vector<uint32_t> child;
        std::unordered_set<uint32_t> usedChild;
        usedChild.clear();
        child.clear();
        uint32_t ch = p.child[chid];
        uint32_t validMode = p.mode[p.modeId(pentry,ch)];
        // printf("validmode: %d, firstmatched vid: %u\n", validMode, firstMatched[validMode]);
        for(auto vid: candidateVid){
            uint32_t t_vid = vid;
            while(t_vid!=INVALID){
                
                uint32_t strid = storeId(t_vid, validMode);

                if(vis[strid] && candidateNum[strid]!=0){
                    
                    // printf("validMode: %d on next: %d->%d, timeCandidateNum: %d, candidate: %d\n", validMode, t_vid, next[strid],timecandidateNum[strid], candidateNum[strid]);
                    // printf("adding child of %d : ",t_vid);
                    uint32_t canNum = candidateNum[strid];
                    for(int cnt=0;cnt<canNum;cnt++){
                        uint32_t canId = candidateId(t_vid, validMode, cnt);
                        uint32_t nxtvid = candidate[canId];
                        if(usedChild.find(nxtvid) == usedChild.end()){
                            child.push_back(nxtvid);
                            usedChild.insert(nxtvid);
                        }
                        // printf(" %d", nxtvid);
                    }
                    // printf("\n");
                    if(next[strid]!=INVALID){
                        uint32_t _strnxt = storeId(next[strid], validMode);
                        prev[_strnxt] = t_vid;
                    }
                }
                
                t_vid = next[strid];
            }
        }
        // memset(visMap, 0, sizeof(uint8_t)*(tnodeCnt*maxPattern));
        StorageGetIntersec(p, ch, child, intersection);
    }
}


void TeGraph::PrevBuild(TePattern *p, uint32_t pentry){
    
    for(uint32_t vid=0; vid<tnodeCnt; vid++){
        for(uint32_t mode=0; mode<maxPattern; mode++){
            uint32_t strid = storeId(vid,mode);
            if(vis[strid] && next[strid]!=INVALID){
                uint32_t vnxt = next[strid];
                if(vnxt != INVALID){
                    uint32_t strnxt = storeId(vnxt, mode);
                    prev[strnxt] = vid;
                }
            }
        }
    }
}

void TeGraph::GetLSG(const TePattern& p, uint32_t gentry){

    std::queue<uint32_t> que;
    que.push(0);
    std::vector<uint32_t> candidate({gentry});

    while (!que.empty())
    {
        uint32_t pnow = que.front();
        que.pop();

        uint32_t cl,cr;
        p.getChild(pnow, cl, cr);
        for(uint32_t cid=cl; cid<cr; cid++){
            uint32_t child = p.child[cid];
            uint32_t mode = p.mode[p.modeId(pnow, child)];
            for(auto vid: candidate){
                uint32_t tvid = vid;
                while (tvid != INVALID)
                {
                    uint32_t strid = storeId(tvid, mode);
                    if(vis[strid] && candidateNum[strid]!=0){
                        uint32_t canNum = candidateNum[strid];
                        for(int cnt=0; cnt<canNum; cnt++){
                            uint32_t canId = candidateId(tvid, mode, cnt);
                            uint32_t vneb = candidate[canId];
                        }
                    }
                }
                
            }
        }

    }
    



}