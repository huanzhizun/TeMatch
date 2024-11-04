import struct
import time
import sys
import os
import random
"""
bin format :
    n1 n2 t1 t2

meaning a edge from (n1, t1) to (n2, t2), t2 >= t1


label format:

tot node
n1 l1
n2 l2
...

"""
import collections

#input txt file, output bin file
def graph_convert(inPath):
    g = collections.OrderedDict()
    tnCnt = 0
    tEdgeCnt=0
    edge = []
    nodemap={}
    nodeCnt=0

    graph = inPath + "/g.txt"
    label = inPath + "label.txt"
    
    with open(graph,'r') as inf :
        outf = open(inPath+"edge.data", 'wb+')
        txf=open(inPath+"temporal.txt",'w+')
        line = inf.readline()
        lcnt=1
        t = time.time()
        while line:
            pair = list(map(int, line.strip('\n').split(' ')))
            # print(pair)
            assert len(pair)>=3
            if len(pair) == 3:
                pair.append(pair[-1]+ random.randint(1,100))
            n1, n2, t1, t2 = pair
            
            if(n1 == n2) : 
                line = inf.readline()
                lcnt += 1
                continue

            assert t1 <= t2, "line {} error".format(lcnt)

            if nodemap.get(n1)==None:
                nodemap[n1]=nodeCnt
                nodeCnt += 1
            if nodemap.get(n2)==None:
                nodemap[n2]=nodeCnt
                nodeCnt += 1

            n1 = nodemap[n1]
            n2 = nodemap[n2]

            if g.get(n1) == None: g[n1] = {}
            if g[n1].get(t1) == None:
                g[n1][t1] = 1
                tnCnt += 1
            if g.get(n2) == None: g[n2] = {}
            if g[n2].get(t2) == None:
                g[n2][t2]=1
                tnCnt += 1
            edge.append((n1,n2,t1,t2))
            tEdgeCnt += 1
            # b = struct.pack('iiii', n1,n2,t1,t2)
            # outf.write(b)
            lcnt+=1
            line = inf.readline()
            # print(lcnt)
        assert tEdgeCnt == len(edge)
        b = struct.pack("i", tEdgeCnt)
        outf.write(b)
        # txf.write("{}\n".format(tEdgeCnt))
        for p in sorted(edge, key=lambda x:(x[0],x[2])):
            n1, n2, t1, t2 = p
            b = struct.pack("iiii",n1,n2,t1,t2)
            outf.write(b)
            txf.write("{} {} {} {}\n".format(n1,n2,t1,t2))
        outf.close()
        txf.close()
        outf = open(inPath+"node.data", 'wb+')
        txf = open(inPath+"node.txt", 'w+')
        l = g.keys()
        b = struct.pack("i", tnCnt)
        outf.write(b)
        txf.write("{}\n".format(tnCnt))
        for k in sorted(l):
            for t in sorted(g[k].keys()):
                b = struct.pack("ii", k,t)
                outf.write(b)
                txf.write("{} {}\n".format(k,t))
                # print(k,t)
        print(tnCnt, tEdgeCnt)
        outf.close()
        txf.close()

    if os.path.exists(label):
        outf = open(inPath+"label.data", 'wb+')
        # txf = open(inPath+"label.txt", 'w+')
        with open(label, "r") as f:
            l = f.readline()
            lb = {}
            lbcnt = 0
            while l:
                l = list(map(int, l.strip("\n").split(" ")))
                n1, l1 = l
                if lb.get(n1) == None:
                    lb[n1] = l1
                    lbcnt+=1
                assert lb[n1]==l1
                l = f.readline()
            outf.write(struct.pack("i",lbcnt))
            for k in sorted(lb.keys()):
                outf.write(struct.pack("ii",k,lb[k]))

    
    print("convert done in : {} s".format(time.time()-t))

def pattern_convert(inPath):
    tnCnt = 0
    node={}
    tEdgeCnt=0
    edge = []
    graph = inPath + "/g.txt"
    label = inPath +  "/label.txt"

    with open(graph,'r') as inf :
        outf = open(inPath+"edge.data", 'wb+')
        line = inf.readline()
        t = time.time()
        while line:
            pair = list(map(int, line.strip('\n').split(' ')))
            n1, n2 = pair
            if node.get(n1)==None:
                node[n1]=1
                tnCnt+=1
            if node.get(n2)==None:
                node[n2]=1
                tnCnt+=1
            edge.append((n1,n2))
            tEdgeCnt += 1
            line = inf.readline()
        b = struct.pack("ii",tnCnt,tEdgeCnt)
        outf.write(b)
        for p in sorted(edge, key=lambda x:(x[0],x[1])):
            n1, n2 = p
            b = struct.pack("ii", n1,n2)
            outf.write(b)
        print("edge: ",tEdgeCnt, " node: ", tnCnt)
        outf.close()

    print(os.path.exists(label))
    print(label)
    if os.path.exists(label):
        outf = open(inPath+"label.data", 'wb+')
        # txf = open(inPath+"label.txt", 'w+')
        with open(label, "r") as f:
            l = f.readline()
            lb = {}
            lbcnt = 0
            while l:
                l = list(map(int, l.strip("\n").split(" ")))
                n1, l1 = l
                if lb.get(n1) == None:
                    lb[n1] = l1
                    lbcnt+=1
                assert lb[n1]==l1
                l = f.readline()
            outf.write(struct.pack("i",lbcnt))
            for k in sorted(lb.keys()):
                outf.write(struct.pack("ii",k,lb[k]))
    
    print("convert done in : {} s".format(time.time()-t))

if __name__ == '__main__':
    args = sys.argv
    print(args)
    assert len(args) == 3, "\nusage : python {} inputDir dataformat[ graph | pattern ]".format(args[0])
    assert args[-1] in ["graph", "pattern"], "\n unknow dataformat, should be graph or pattern"
    if args[-1]=="graph": graph_convert(args[1])
    elif args[-1]=="pattern": pattern_convert(args[1])

    # args = sys.argv
    # assert len(args) == 3,"\nusage : python {} inputFile outputDir".format(args[0])

    # inPath=args[1]
    # inPath=args[2]

    # cnt=0
    # node=[-1 for i in range(2000)]
    # dic = {}
    # with open(inPath, 'r') as f:
    #     l=f.readline()
    #     while l:
    #         pair = list(map(int, l.strip('\n').split(' ')))
    #         n1,n2,t = pair
    #         node[n1]=node[n2]=1
    #         if dic.get(n1) ==None: dic[n1]={}
    #         if dic[n1].get(n2)==None: dic[n1][n2]=1
    #         l=f.readline()
    
    # __map=[-1 for _ in range(2000)]
    # for i,n in enumerate(node):
    #     if(n==1):
    #         __map[i]=cnt
    #         cnt+=1
    # # print(__map)
    # with open(inPath, 'r') as f:
    #     l=f.readline()
    #     compat = open(outPath+"data.out",'w+')
    #     while l:
    #         pair = list(map(int, l.strip('\n').split(' ')))
    #         n1,n2,t = pair
    #         n1 = __map[n1]
    #         n2=__map[n2]
    #         compat.write("{} {} {}\n".format(n1,n2,t))
    #         l=f.readline()
    #     compat.close()
    
    # with open(outPath+"static.txt",'w+') as f:
    #     for k in sorted(dic.keys()):
    #         for v in sorted(dic[k].keys()):
    #             f.write("{} {}\n".format(__map[k],__map[v]))
    