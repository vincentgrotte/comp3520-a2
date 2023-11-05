BUILD := gcc -pthread \
	main.c \
	mab.c \
	pcb.c \
	-o solution

build:
	$(BUILD)

build-all: build
	gcc random.c -o random -lm
	gcc sigtrap.c -o process

check:
	python3 check_results.py ./tests/jobs.txt ./output/output.txt

check-verbose:
	VERBOSE=1 python3 check_results.py ./tests/jobs.txt ./output/output.txt

debug:
	$(BUILD) -Wall
	./solution ./tests/jobs-1.txt 3 3 5 | tee output/output-1.txt

roll:
	./random test-input.txt

test: build
	./solution ./tests/jobs-2.txt 3 5 2 | tee output/output-2.txt

test-all: build
	./solution ./tests/jobs-1.txt 3 5 2 | tee output/output-1.txt
	python3 check_results.py ./tests/jobs-1.txt ./output/output-1.txt
	
run: build
	./solution
