BUILD := gcc -pthread \
	main.c \
	mab.c \
	pcb.c \
	-o solution

# ----------------------------------------------------------
.PHONY: build
build:
	$(BUILD)

# ----------------------------------------------------------
.PHONY: build-all
build-all: build
	gcc random.c -o random -lm
	gcc sigtrap.c -o process

# ----------------------------------------------------------
.PHONY: check
check:
	python3 check_results.py ./tests/jobs.txt ./output/output.txt

# ----------------------------------------------------------
.PHONY: check-verbose
check-verbose:
	VERBOSE=1 python3 check_results.py ./tests/jobs.txt ./output/output.txt

# ----------------------------------------------------------
.PHONY: debug
debug:
	$(BUILD) -Wall
	./solution ./tests/jobs.txt 3 5 2 | tee output/output.txt

# ----------------------------------------------------------
.PHONY: roll
roll:
	./random test-input.txt

# ----------------------------------------------------------
.PHONY: test
test: build
	./solution ./tests/jobs.txt 3 5 2 | tee output/output.txt

# ----------------------------------------------------------
.PHONY: test-all
test-all: build
	./solution ./tests/jobs.txt 3 5 2 | tee output/output.txt
	./solution ./tests/jobs-1.txt 3 5 2 | tee output/output-1.txt
	./solution ./tests/jobs-2.txt 3 5 2 | tee output/output-2.txt
	./solution ./tests/jobs-3.txt 3 5 2 | tee output/output-3.txt

# ----------------------------------------------------------
.PHONY: test-flow
test-flow: build
	./solution ./tests/jobs.txt 3 5 2 | tee output/output.txt
	python3 check_results.py ./tests/jobs.txt ./output/output.txt

# ----------------------------------------------------------
.PHONY: run
run: build
	./solution

# ----------------------------------------------------------
.PHONY: val
val: build
	valgrind ./solution ./tests/jobs.txt 3 5 2

# ----------------------------------------------------------
.PHONY: val
val-big: build
	valgrind --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file=valgrind-out.txt \
        ./solution ./tests/jobs.txt 3 5 2


