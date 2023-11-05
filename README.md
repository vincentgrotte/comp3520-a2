SID: 309193168

To build and run the module do:

- make build
- ./solution <job-list-file>

To run the sanity checker:

python3 check_results.py ./tests/jobs.txt ./output/output.txt

Use VERBOSE=1 to get more information

Notes on test jobs:

start with 0, 0
increasing arrival times
range-bound, increasing service times
throw in a few zeros
have jobs comfortably in the middle of a range
have jobs closer to the edges of the range
    for example: one second on either side of the cutoff


