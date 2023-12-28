
WARN_FLAGS= -Werror -Wall -Wextra -pedantic -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wstrict-overflow=4 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option
FLAGS = -ggdb

DEPS = fformat.h
SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c, %, $(SRCS))

all: build Makefile

build: $(PROGS) Makefile

%: %.c Makefile $(DEPS)
	gcc -o $@ $< $(FLAGS) $(WARN_FLAGS)
	@echo "built $@"

clean:
	rm $(PROGS)
	rm test.friendly beautiful.map
