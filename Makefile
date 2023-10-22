BUILD := gcc -pthread \
	main.c \
	some-routine.c \
	test-params.c \
	-o solution

build:
	$(BUILD)

debug:
	$(BUILD) -Wall
	./solution -d

test:
	$(BUILD)
	./solution -d

run:
	$(BUILD)
	./solution
