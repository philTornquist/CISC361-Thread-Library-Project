# Phil Tornquist
# Sean Moir
# CISC361-010
# Project 4

CC = gcc
CFLAGS = -g -I.. -I. -LEVEL_2_QUEUE

# specify the object files
LIBOBJS = t_lib.o 
# specify the source files
LIBSRCS = t_lib.c

default: t_lib.a

all: t_lib.a tests

t_lib.a: $(LIBOBJS) Makefile
	ar rcs t_lib.a $(LIBOBJS)	
# ar creates the static thread library

t_lib.o: t_lib.c t_lib.h Makefile
	$(CC) $(CFLAGS) -c t_lib.c

# make all test files located in "Tests" directory
tests: $(patsubst Tests/%.c, Tests/%, $(wildcard Tests/*.c)) t_lib.a Makefile

Tests/%: Tests/%.c t_lib.a Makefile
	$(CC) $(CFLAGS) -o $@ $< t_lib.a

clean:
	rm -rf t_lib.a $(LIBOBJS) $(patsubst Tests/%.c, Tests/%, $(wildcard Tests/*.c))

.PHONY: all clean default tests