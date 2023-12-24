
libs =
flags = -Werror -Wall -pedantic -ggdb
srcs = $(wildcard *.c)
output = main

all: run

run: build
	./$(output)

build: $(srcs)
	gcc -o $(output) $(srcs) $(flags) $(libs)

clean:
	rm $(output)
