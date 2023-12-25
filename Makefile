
flags = -Werror -Wall -pedantic -ggdb
srcs = $(wildcard *.c)
output = main

all: test

test: build
	./$(output)

build: $(srcs)
	gcc --std=c99 -o $(output) $(srcs) $(flags)

clean:
	rm $(output)
