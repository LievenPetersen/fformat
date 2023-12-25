
warn-flags=-Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wstrict-overflow=4 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option
flags = -Werror -Wall -pedantic -ggdb
srcs = $(wildcard *.c)
output = testrun

all: test

test: build
	./$(output)

build: $(srcs)
	gcc --std=c99 -o $(output) $(srcs) $(flags) $(warn-flags)

clean:
	rm $(output)
