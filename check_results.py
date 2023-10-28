import re

# Read test input
input_file = open("test-input.txt", "r")
input_lines = input_file.readlines()

print("# Job definitions #")

definitions = []

# Scan test input and gather definitions
for input_line in input_lines:
    clean_line = input_line.strip()
    print(clean_line)
    clean_line_chunks = clean_line.split(",")
    output_arrival_time = clean_line_chunks[0].strip()
    output_service_time = clean_line_chunks[1].strip()
    definitions.append((output_arrival_time, output_service_time))

# Read test output
output_file = open("test-output.txt", "r")
output_lines = output_file.readlines()

print("# Results #")

results = []
completed = 0

# Scan test output and gather results
for output_line in output_lines:
    clean_line = output_line.strip()

    if "Level-0 max cpu time:" in clean_line:
        param_t0 = clean_line[22]
        print(f"# param_t0: {param_t0}")

    if "Level-1 max cpu time:" in clean_line:
        param_t1 = clean_line[22]
        print(f"# param_t1: {param_t1}")

    if "Level-1 queue max iterations:" in clean_line:
        param_k = clean_line[30]
        print(f"# param_k: {param_k}")

    if "[*]" in clean_line:
        # print(clean_line)

        # Check completed jobs
        if "Completed" in clean_line:
            completed += 1

            match = re.search("\[\*\] L(\d+)", clean_line)
            input_queue = match.group(1) if match else None
            match = re.search("::[0-9]{4} \[(\d+)", clean_line)
            input_timer = match.group(1) if match else None
            match = re.search("A:(\d+)", clean_line)
            input_arrival_time = match.group(1) if match else None
            match = re.search("S:(\d+)", clean_line)
            input_service_time = match.group(1) if match else None
            match = re.search("T:(\d+)", clean_line)
            input_turnaround = match.group(1) if match else None
            match = re.search("W:(\d+)", clean_line)
            input_wait = match.group(1) if match else None

            print(
                f"TIME:{input_timer} "
                f"Q:{input_queue} "
                f"A:{input_arrival_time} "
                f"S:{input_service_time} "
                f"T:{input_turnaround} "
                f"W:{input_wait}"
            )

# Check input against results
if completed == len(definitions):
    print(f"Success: {completed} jobs completed")



