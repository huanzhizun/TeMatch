
import subprocess
import re
import resource
import sys
from datetime import datetime
import signal
import os


def run(query, data, program, binroot, dataroot, thread_cnt, timeout = 60*60*1):
    
    print(f"starting matching at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print(f"running with thread num {thread_cnt}")

    size = query[0]
    adj = query[1]
    app = program[0]
    pattern = program[1]

    baseCmd = ["/usr/bin/time", "-v", binroot+f"{app}","SELF_DEFINED",dataroot+f"{data}",f"{size}",f"{adj}", f'{thread_cnt}']

    proc = subprocess.Popen(baseCmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, preexec_fn=os.setpgrp)

    try:
        stdout, stderr = proc.communicate(timeout = timeout)  # 等待进程完成

        if proc.returncode != 0:
            raise subprocess.CalledProcessError(proc.returncode, baseCmd)
        
        # print(stdout)
        match = re.search(pattern, stdout)

        if match:
            if(app == "test"):
                values = {
                    "ans": int(match.group(2)),
                    "TCS_DP": float(match.group(3)),
                    "mean_matchTime": float(match.group(1)),
                    "matchTime": float(match.group(4)),
                    "end2end": float(match.group(5))
                }
                # print(values)
            elif app == 'unit_test':
                values = {
                    "root":int(match.group(1)),
                    "ans":int(match.group(2)),
                    "TCStime":float(match.group(3)),
                    "mean_Matchtime":float(match.group(4)),
                    "tot":float(match.group(5)),
                    "end2end":float(match.group(6))
                }
            elif app == "memory":
                values = {
                    "memory": float(match.group(1))
                }
            else:
                values = {
                    "ans": int(match.group(1)),
                    "time": float(match.group(2))
                }
        for k in values.keys():
            print(f'{k} : {values[k]}')
        # print(f"peakmem_KB : {peakmem_KB} KB")

        memory = re.search(r"Maximum resident set size \(kbytes\): (\d+)", stderr)
        if(memory):
            print("peak memory use (MB) : ", int(memory.group(1))/1024)
        exitState = re.search(r'Exit status: (\d+)', stderr)
        if(exitState):
            print("exit status : ", int(exitState.group(1)))        

    except subprocess.CalledProcessError as e:
        # 如果命令执行失败（返回非零状态码），则打印标准错误输出
        print(f"Command failed with error:\n")
        print("stderr :", e.stderr)
        print(f"stdout : {e.stdout}")

    except subprocess.TimeoutExpired:
        os.killpg(os.getpgid(proc.pid), signal.SIGTERM)
        print("proc timeout and try terminating...")
        try:
            print("wating for terminate")
            proc.wait(timeout = 5)
        except subprocess.TimeoutExpired:
            print("wait timeout, try killing")
            os.killpg(os.getpgid(proc.pid), signal.SIGKILL)
        print("proc terminated")
        outs, errs = proc.communicate()
        print(f"Command exceeded {timeout} seconds!")

    except Exception as e:
        # 捕获其他可能的异常
        print(f"An error occurred: {str(e)}")

    finally:
        print(f"end at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")





if __name__ == '__main__':

    import os

    sys.stdout = open(sys.stdout.fileno(), mode='w', encoding='utf8', buffering=1)
    sys.stderr = open(sys.stderr.fileno(), mode='w', encoding='utf8', buffering=1)

    script_path = os.path.realpath(__file__)

    # 获取当前脚本所在的目录
    script_dir = os.path.dirname(script_path)

    timeout = 60 * 60 * 12

    print("python file is in the :", script_dir)

    querys = {
        'P1': {
            3: [4, '0111001100010000']
        },
        'P2': {
            3: [5, '0110000111000110000000000'], 
            4: [5, '0110000111000100000000100']
        },
        'P3': {
            4: [5, '0110000111000110000100000']
        },
        'P4': {
            4: [5, '0111100111000110000100000']
        },
        'P5': {
            4: [6, '011000001111000111000010000000000010'], 
            5: [6, '011000001111000111000010000001000000']
        },
        'P6': {
            4: [6, '011000001110000011000010000001000000'], 
            5: [6, '011000001110000011000000000100000010']
        },
        'P7': {
            3: [6, '011000000100000111000011000000000000'], 
            4: [6, '011000000100000111000010000000000100'], 
            5: [6, '011000000100000010001011000000001000']
        },
        'P8': {
            5: [7, '0111111001111100011110000111000001100000000000000'], 
            6: [7, '0111111001111100011110000111000001000000000000100']
        }
    }

    #test_pattern = r"average_matchTime: ([\d.]+) ans (\d+) tcsDP ([\d.]+) matchTime ([\d.]+) end2end ([\d.]+)"
    baseline_pattern = r'ans (\d+) time ([\d.]+)'
    unit_pattern = r'root (\d+) ans (\d+) TCStime ([\d.]+) Matchtime ([\d.]+) tot ([\d.]+) end2end ([\d.]+)'

    #memory_pattern = r'total_mem MB: ([\d.]+)'


    binroot = f"{script_dir}/build/bin/"
    program = [
        # ["test", test_pattern],
    #    ["baseline", baseline_pattern]
       ['unit_test', unit_pattern]
        # ["memory", memory_pattern]
    ]

    dataroot = f"{script_dir}/dataset/"
    dataset = [
        # "collegeMsg/", 
        # "bmc/",
        # "email/",
        "askubuntu/",
        # "superuser/",
        # "wiki_talk/",
        # "stackoverflow/"
    ]

    query_len = {1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4,5], 8: [5,6]}

    queryList = {
        # "collegeMsg/":{1: [3], 2: [3,4]},
        "collegeMsg/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4,5]},
        "bmc/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4], 8: [5,6]},
        "email/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 7: [3,4], 8: [5]},
        "askubuntu/":{8: [6]},
        # "askubuntu/":{7: [4]},
        "superuser/":{1: [3], 2: [3,4], 3: [4], 4: [4], 5: [4,5], 6: [4,5], 8: [5,6]},
        "wiki_talk/":{1: [3], 2: [3,4], 3: [4], 4: [4]}
    }

    running_thread = {
        "bmc/": 40,
        "email/": 40,
        "superuser/": 40,
        "askubuntu/": 40,
        "collegeMsg/": 40,
        "wiki_talk/":40,
        "stackoverflow/":40
    }

    # testthread = [
    #     40,
    #     24,
    #     16,
    #     8,
    #     4,
    #     2,
    #     # 1
    # ]

    # for thread in testthread:
    for data in dataset:
        for queryNum, len_lists in queryList[data].items():
            query = "P"+str(queryNum)
            # for max_len, pattern in querys[query].items():
            for max_len in len_lists:
                pattern = querys[query][max_len]
                print("-"*30)
                print(f"querying pattern: {query}/{max_len}.puml")
                print("-"*30)
                for app in program:
                        # print("+"*10, f"{app[0]} on {data}" ,"+"*15, sep=" ")
                        # run(pattern, data, app, binroot, dataroot, thread, timeout)
                    print("+"*10, f"{app[0]} on {data}" ,"+"*15, sep=" ")
                    run(pattern, data, app, binroot, dataroot, 1, timeout)



# nohup python ../../evaluation.py > ../../test_log 2>&1 < /dev/null &
