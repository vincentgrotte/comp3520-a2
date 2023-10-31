BUILD := gcc -pthread \
	main.c \
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
	./solution ./tests/jobs.txt 3 3 5 | tee output/output.txt

test-all: build
	./solution ./tests/jobs-1.txt 5 5 2 | tee output/output-1.txt
	./solution ./tests/jobs-2.txt 3 3 5 | tee output/output-2.txt
	./solution ./tests/jobs-3.txt 3 3 5 | tee output/output-3.txt
	./solution ./tests/jobs-4.txt 3 3 5 | tee output/output-4.txt
	./solution ./tests/jobs-5.txt 3 3 5 | tee output/output-5.txt
	python3 check_results.py ./tests/jobs-1.txt ./output/output-1.txt
	python3 check_results.py ./tests/jobs-2.txt ./output/output-2.txt
	python3 check_results.py ./tests/jobs-3.txt ./output/output-3.txt
	python3 check_results.py ./tests/jobs-4.txt ./output/output-4.txt
	python3 check_results.py ./tests/jobs-5.txt ./output/output-5.txt

run: build
	./solution
