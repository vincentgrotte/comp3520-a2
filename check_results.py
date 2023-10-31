import os
import re
import sys


def verbose_print(*args):
    if (os.environ.get("VERBOSE") == "1"):
        print(*args)


if not len(sys.argv) == 3:
    raise "Usage: <jobs_file_path> <output_file_path>"

jobs_file_path = sys.argv[1]
output_file_path = sys.argv[2]

# Read test input
jobs_file = open(jobs_file_path, "r")
print("Checking output from", sys.argv[1])
input_lines = jobs_file.readlines()

jobs = [
    ( # list of tuples
        input_line.strip().split(",")[0].strip(), # arrival_time
        input_line.strip().split(",")[1].strip() # service_time
    ) for input_line in input_lines
]

verbose_print("jobs")
for job in jobs: verbose_print(job)

# Read test output
output_file = open(output_file_path, "r")
output_lines = output_file.readlines()

# Get params
match = re.search("L0Q max time: (\d+)", output_lines[1].strip())
if match: param_t0 = int(match.group(1))

match = re.search("L1Q max time: (\d+)", output_lines[2].strip())
if match: param_t1 = int(match.group(1))

match = re.search("L1Q max iterations: (\d+)", output_lines[3].strip())
if match: param_k = int(match.group(1))

verbose_print("params")
verbose_print("L0Q max time:", param_t0)
verbose_print("L1Q max time:", param_t1)
verbose_print("L1Q max iterations:", param_k)
verbose_print("Total time:", param_t0 + param_t1 * param_k)

# Get all pid logs
pid_logs = {}

for x, output_line in enumerate(output_lines):
    pid = None
    arrive = None
    service = None
    cpu = None
    status = None
    start_cmd = None
    stop_cmd = None

    clean_line = output_line.strip()
    pid_log_header_line = re.search("pid[ ]*arrive[ ]*service[ ]*cpu[ ]*status", clean_line)
    
    if pid_log_header_line:
        pid_log_line = output_lines[x + 1].split()
        
        pid = pid_log_line[0].strip()
        pid_log = {
            "arrive": int(pid_log_line[1].strip()),
            "service": int(pid_log_line[2].strip()),
            "cpu": int(pid_log_line[3].strip()),
        }

        if not pid_logs.get(pid):
            pid_logs[pid] = [pid_log]
        else:
            pid_logs[pid].append(pid_log)

pids = list(pid_logs.keys())
verbose_print("pids")
verbose_print(pids)

results = {}

verbose_print("check results")
for pid in pid_logs.keys():
    verbose_print(pid)

    if not pid_logs[pid]:
        verbose_print("no logs for", pid)
        continue

    arrive = pid_logs[pid][0]["arrive"]
    service = pid_logs[pid][0]["service"]

    should_finish_in_L0 = service < param_t0
    should_finish_in_L1 = service < param_t0 + (param_t1 * param_k)
    should_finish_in_L2 = service >= param_t0 + (param_t1 * param_k)

    is_success = False
    is_edge = False

    if should_finish_in_L0 and len(pid_logs[pid]) == 1:
        is_success = True
        verbose_print("[SUCCESS] correctly finished in L0")

    elif should_finish_in_L1 and len(pid_logs[pid]) <= 1 + param_k:
        is_success = True
        verbose_print("[SUCCESS] correctly finished in L1")

    elif should_finish_in_L2 and len(pid_logs[pid]) > 1 + param_k:
        is_success = True
        verbose_print("[SUCCESS] correctly finished in L2")

    # Edge cases
    elif should_finish_in_L0 and len(pid_logs[pid]) <= 1 + param_k:
        is_success = True
        is_edge = True
        verbose_print("[SUCCESS] should have finished in L0 but finished in L1")

    elif should_finish_in_L1 and len(pid_logs[pid]) >= 1 + param_k:
        is_success = True
        is_edge = True
        verbose_print("[SUCCESS] should have finished in L1 but finished in L2")

    else:
        verbose_print("[FAILED] something went wrong")

    for pid_log in pid_logs[pid]:
        verbose_print(pid_log)  

    results[pid] = (is_success, is_edge)

success_count = 0
edge_count = 0
for pid, (is_success, is_edge) in results.items():
    status = "FAILED"
    if is_success:
        success_count += 1
        status = "SUCCESS"
    if is_edge:
        edge_count += 1
        status = "EDGE"
    print(f"{pid} [{status}]")

print(f"{success_count}/{len(pids)} Succeeded")
print(f"{edge_count}/{len(pids)} Edge Cases")


