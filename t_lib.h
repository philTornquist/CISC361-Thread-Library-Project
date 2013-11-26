/*
 Phil Tornquist
 Sean Moir
 CISC361-010
 t_lib.h
*/

#ifndef T_LIB_H
#define T_LIB_H

#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>

struct tcb {
  int         thread_id;
  int         thread_priority;
  ucontext_t  thread_context;
  mbox *mbox;
  struct tcb *next;
};

typedef struct tcb tcb;


/* Thread library function prototypes */
void t_create(void (*fct)(void), int id, int pri);
void t_init();
void t_shutdown();
int t_terminate();
void t_yield();
void t_block(tcb **queue_start, tcb **queue_end);

extern tcb *running;

struct thread_t
{
  tcb *tcb;
  struct thread_t *next; 
};

typedef struct thread_t thread_t;

#endif /* T_LIB_H */
