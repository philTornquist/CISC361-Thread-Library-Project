/*
 * types used by thread library
 */

#ifndef T_LIB_H
#define T_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

int t_create(void (*fct)(void), int id, int pri);
void t_init();
void t_shutdown();
int t_terminate();
void t_yield();

#endif /* T_LIB_H */
