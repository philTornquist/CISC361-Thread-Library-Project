# Phil Tornquist
# Sean Moir
# CISC361-010
# Project 4

CC = gcc
CFLAGS = -g -I.. -I.
LIBOBJS = t_lib.o sem.o mbox.o
LIBDEPS = t_lib.h sem.h mbox.h

default: t_lib.a

all: t_lib.a tests

t_lib.a: $(LIBOBJS) Makefile
	ar rcs t_lib.a $(LIBOBJS)	
# ar creates the static thread library

%.o: %.c $(LIBDEPS) Makefile
	$(CC) $(CFLAGS) -c $<

# make all test files located in "Tests" directory
tests: $(patsubst Tests/%.c, Tests/%, $(wildcard Tests/*.c)) t_lib.a Makefile

Tests/%: Tests/%.c t_lib.a Makefile
	$(CC) $(CFLAGS) -o $@ $< t_lib.a

clean:
	rm -rf t_lib.a $(LIBOBJS) $(patsubst Tests/%.c, Tests/%, $(wildcard Tests/*.c))

.PHONY: clean all default tests
