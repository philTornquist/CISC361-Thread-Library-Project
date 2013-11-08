/*
 Phil Tornquist - 701526283
 Sean Moir      - 701411108
 CISC361-010
 t_lib.c
*/

#include "t_lib.h"
#include <stdlib.h>

struct tcb {
  int         thread_id;
  int         thread_priority;
  ucontext_t  thread_context;
  struct tcb *next;
};

typedef struct tcb tcb;

tcb *running;
tcb *end_queue;

/*
 * t_queue()
 * Add thread to end of ready queue 
 */
void t_queue(tcb *thread)
{
  end_queue->next = thread;
  end_queue = thread;
  end_queue->next = NULL;
}

/* 
 * t_init()
 * Initialize thread library 
 */
void t_init()
{
  tcb *tmp;
  tmp = (tcb *) malloc(sizeof(tcb));
  tmp->next = NULL;
  tmp->thread_priority = 1;

  /* let tmp be the context of main() */
  getcontext(&tmp->thread_context);

  running = tmp;
  end_queue = tmp;
}

/* 
 * t_shutdown()
 * Shut down thread library 
 */
void t_shutdown()
{
  while (running != NULL) {
    tcb *tmp = running;
    running = running->next;
    free(tmp->thread_context.uc_stack.ss_sp);
    free(tmp);
  }
}

/*
 * start_thread()
 */
void start_thread(int id, void (*fct)(int))
{
  fct(id);
  if (t_terminate() == -1) {
    t_shutdown();
    exit(0);
  }
}

/* 
 * t_create()
 * Create new thread 
 */
void t_create(void (*fct)(void), int id, int pri)
{
  size_t sz = 0x10000;

  tcb *uc;
  uc = (tcb *) malloc(sizeof(tcb));
  uc->thread_priority = pri;
  uc->thread_id = id;

  getcontext(&uc->thread_context);
  uc->thread_context.uc_stack.ss_sp = malloc(sz);
  uc->thread_context.uc_stack.ss_size = sz;
  uc->thread_context.uc_stack.ss_flags = 0;
  makecontext(&uc->thread_context, start_thread, 2, id, fct);

  t_queue(uc);
}

/* 
 * t_terminate()
 * Terminate currently running thread 
 */
int t_terminate()
{
  tcb *tmp = running;
  running = running->next;

  if (running == NULL) {
    running = tmp;
    end_queue = running;
    return -1;
  }

  free(tmp->thread_context.uc_stack.ss_sp);
  free(tmp);
  
  setcontext(&running->thread_context);
}

/* 
 * t_yield()
 * Move currently running thread to end of ready queue, start up next ready thread 
 */
void t_yield()
{
  tcb *tmp = running;
  running = running->next;

  if (running == NULL) {
    running = tmp;
    end_queue = running;
    return;
  }

  t_queue(tmp);

  swapcontext(&tmp->thread_context, &running->thread_context);
}
