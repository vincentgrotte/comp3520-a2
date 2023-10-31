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

# test-all: build
# 	./solution ./tests/test-1.txt 3 3 5 | tee output/output-1.txt
# 	./solution ./tests/test-2.txt 3 3 5 | tee output/output-2.txt
# 	./solution ./tests/test-3.txt 3 3 5 | tee output/output-3.txt
# 	./solution ./tests/test-4.txt 3 3 5 | tee output/output-4.txt
# 	./solution ./tests/test-5.txt 3 3 5 | tee output/output-5.txt
# 	./solution ./tests/test-6.txt 3 3 5 | tee output/output-6.txt
# 	./solution ./tests/test-7.txt 3 3 5 | tee output/output-7.txt
# 	./solution ./tests/test-8.txt 3 3 5 | tee output/output-8.txt
# 	./solution ./tests/test-9.txt 3 3 5 | tee output/output-9.txt
# 	./solution ./tests/test-10.txt 3 3 5 | tee output/output-10.txt

run: build
	./solution
