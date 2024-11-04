## TeMatch

TeMatch is a high-performance framework for temporal subgraph matching. It features a novel topological representation of temporal constraints in the query graph, along with three temporal-aware subgraph matching algorithms that facilitate rapid constraint checking, early pruning, and efficient enumeration. Extensive experiments demonstrate that TeMatch effectively utilizes temporal information to reduce the search space, achieving speedups of up to two orders of magnitude while being parallel-friendly, highly compatible, and producing identical matching results.

## How to run 

### Step 1 : Load the dataset

The experimental datasets are stored in the dataset folder and include askubuntu, bmc, collegeMsg, email, stackoverflow, superuser, wiki_talk, and orkut. The bmc dataset can be obtained from its official website, while askubuntu, collegeMsg, email, stackoverflow, superuser, wiki_talk, and orkut are available from the [Stanford Large Network Dataset Collection] (http://snap.stanford.edu./data/index.html)

After downloading the source data, place it in the dataset folder and name it g.txt. Use dataConvert.py to process the temporal edges. Due to the large size of some datasets, processing may be paused. The input parameters for dataConvert.py include the dataset folder location and the conversion type. For example, for the bmc dataset, the input parameters are ./bmc/ and graph. Conversion types include graph and pattern. The graph conversion generates temporal.txt, node.txt, edge.data, and node.data.

```
cd dataset

nohup python dataConvert.py ./bmc/ graph > log_bmc.txt 2>&1 &
```
After that, enter convert.ipynb in the dataset folder, change the dataset path, and run the code block to generate static.txt.
```python
path = r'./bmc/'
```

### Step 2 : Evaluate models

After configuring the dataset, the operations in the terminal are as follows:

#### Compilation

```
cd build

cmake ..

make 

cd ..
```


#### Evaluation

Due to the large size of some datasets, the processing can be suspended.
```
nohup python ../../evaluation.py > ../../test_log 2>&1 < /dev/null &
```
In evaluation.py

##### Change the model

baseline stands for GraphPi；unit_test stands for Tematch
```python
program = [
        # ["test", test_pattern],
        ["baseline", baseline_pattern]
       #['unit_test', unit_pattern]
        # ["memory", memory_pattern]
   ]
```
##### Change the dataset 

```python

dataset = [

        # "collegeMsg/", 
    
         "bmc/",
    
        # "email/",

        #"askubuntu/",

        # "superuser/",
    
        # "wiki_talk/",
    
         #"stackoverflow/",

        #"orkut/"
    ]
```

##### Change the pattern of the dataset 
```python
queryList = {
        # "collegeMsg/":{1: [3], 2: [3,4]},
        #"collegeMsg/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4,5]},
        "bmc/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4], 8: [5,6]},
        #"email/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4], 8: [5]},
        #"askubuntu/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4,5], 8: [5,6]},
        #"superuser/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 8: [5,6]},
       #"wiki_talk/":{1: [3], 2: [3,4], 3: [4], 4: [4]}
       #"stackoverflow/":{1: [3], 2: [3],  4: [4], 5: [5], 6: [5]}
        #"orkut/":{1: [3], 2: [3],  4: [4], 5: [5], 6: [5]}
    }
```
The fourth parameter of the run function referenced in the main function can be used to adjust the number of threads. Please fill it in according to the actual situation; the timeout is set to 12 hours, but for some large datasets, the time may not be enough and it can be set to 1 week.
