# Makefile for UD CISC361 user-level thread library

CC = gcc
CFLAGS = -g -I.. -I.

LIBOBJS = t_lib.o 

# specify the source files
LIBSRCS = t_lib.c


default: t_lib.a

t_lib.a: ${LIBOBJS} Makefile
	ar rcs t_lib.a ${LIBOBJS}	
# ar creates the static thread library

# here, we specify how each file should be compiled, what
# files they depend on, etc.

t_lib.o: t_lib.c t_lib.h Makefile
	${CC} ${CFLAGS} -c t_lib.c

tests: $(patsubst Tests/%.c, Tests/%, $(wildcard Tests/*.c))

Tests/%: Tests/%.c
	$(CC) $(CFLAGS) -o $@ $< t_lib.a

clean:
	rm -rf t_lib.a ${LIBOBJS} Tests/%

.PHONY: clean default tests