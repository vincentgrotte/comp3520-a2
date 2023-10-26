BUILD := gcc -pthread \
	main.c \
	pcb.c \
	-o solution

build:
	$(BUILD)

build-all: build
	gcc random.c -o random -lm
	gcc sigtrap.c -o process

debug:
	$(BUILD) -Wall
	./solution -d

roll:
	./random test-input.txt

test: build
	./solution ./test-input.txt -t

run: build
	./solution test-input.txt
