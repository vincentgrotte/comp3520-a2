BUILD := gcc -pthread \
	main.c \
	pcb.c \
	test-params.c \
	-o solution

build:
	$(BUILD)

build-all: build
	gcc random.c -o random -lm
	gcc sigtrap.c -o process

debug:
	$(BUILD) -Wall
	./solution test-input.txt -t

roll:
	./random test-input.txt

test: build
	./solution test-input.txt -t

test-record: build
	./solution test-input.txt -t > test-output.txt

run: build
	./solution test-input.txt
