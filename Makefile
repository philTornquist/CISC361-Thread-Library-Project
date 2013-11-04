# Phil Tornquist
# Sean Moir
# CISC361-010
# Project 4

# Makefile for UD CISC361 user-level thread library

CC = gcc
CFLAGS = -g -I.. -I. -LEVEL_2_QUEUE

LIBOBJS = t_lib.o 

# specify the source files
LIBSRCS = t_lib.c


default: t_lib.a

all: t_lib.a tests

t_lib.a: ${LIBOBJS} Makefile
	ar rcs t_lib.a ${LIBOBJS}	
# ar creates the static thread library

t_lib.o: t_lib.c t_lib.h Makefile
	${CC} ${CFLAGS} -c t_lib.c

tests: $(patsubst Tests/%.c, Tests/%, $(wildcard Tests/*.c)) t_lib.a Makefile

Tests/%: Tests/%.c t_lib.a Makefile
	$(CC) $(CFLAGS) -o $@ $< t_lib.a

clean:
	rm -rf t_lib.a ${LIBOBJS}

.PHONY: all clean default tests