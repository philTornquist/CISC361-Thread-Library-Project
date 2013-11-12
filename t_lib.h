/*
 Phil Tornquist
 Sean Moir
 CISC361-010
 t_lib.h
*/

#ifndef T_LIB_H
#define T_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

/* Thread library function prototypes */
void t_create(void (*fct)(void), int id, int pri);
void t_init();
void t_shutdown();
int t_terminate();
void t_yield();
void t_block(tcb **queue_start, tcb **queue_end);

#endif /* T_LIB_H */
