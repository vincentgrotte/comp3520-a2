BUILD := gcc -pthread \
	main.c \
	mab.c \
	pcb.c \
	-o solution

# Build the solution
.PHONY: build-base
build-base:
	$(BUILD)

# Build the solution with -Wall
.PHONY: build-strict
build-strict:
	$(BUILD) -Wall

# Build the random module
.PHONY: build-random
build-random:
	gcc random.c -o random -lm

# Build sigtrap and the solution
.PHONY: build
build: build-base
	gcc sigtrap.c -o process

# Generate some random jobs
.PHONY: roll
roll:
	./random random-jobs.txt

# Run the solution against a test file
.PHONY: test
test: build
	./solution ./tests/jobs.txt 3 5 2 | tee output/output.txt

# Run the solution against multiple test files
.PHONY: test-all
test-all: build
	./solution ./tests/jobs.txt 3 5 2 | tee output/output.txt
	./solution ./tests/jobs-1.txt 3 5 2 | tee output/output-1.txt
	./solution ./tests/jobs-2.txt 3 5 2 | tee output/output-2.txt
	./solution ./tests/jobs-3.txt 3 5 2 | tee output/output-3.txt

# Run run the solution against a test file through valgrind
.PHONY: val
val: build
	valgrind ./solution ./tests/jobs.txt 3 5 2

# Run run the solution against a test file through valgrind with lots of flags
.PHONY: val-big
val-big: build
	valgrind --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        --verbose \
        --log-file=valgrind-out.txt \
        ./solution ./tests/jobs.txt 3 5 2


