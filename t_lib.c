#include "t_lib.h"

struct tcb {
  int         thread_id;
  int         thread_priority;
  ucontext_t  thread_context;
  struct tcb *next;
};

typedef struct tcb tcb;

tcb *running;
tcb *end_queue;

/* Add thread to end of ready queue */
void t_queue(tcb *thread)
{
  end_queue->next = thread;
  end_queue = thread;
  end_queue->next = NULL;
}

/* Initialize thread library */
void t_init()
{
  tcb *tmp;
  tmp = (tcb *) malloc(sizeof(tcb));
  tmp->next = NULL;

  /* let tmp be the context of main() */
  getcontext(&tmp->thread_context);
  running = tmp;

  end_queue = tmp;
}

/* Shut down thread library */
void t_shutdown()
{
  while (running != NULL) {
    tcb *tmp = running;
    running = running->next;
    free(tmp);
  }
}

/* Create new thread */
int t_create(void (*fct)(void), int id, int pri)
{
  size_t sz = 0x10000;

  tcb *uc;
  uc = (tcb *) malloc(sizeof(tcb));

  getcontext(&uc->thread_context);
  uc->thread_context.uc_stack.ss_sp = mmap(0, sz,
       PROT_READ | PROT_WRITE | PROT_EXEC,
       MAP_PRIVATE | MAP_ANON, -1, 0);
  uc->thread_context.uc_stack.ss_size = sz;
  uc->thread_context.uc_stack.ss_flags = 0;
  uc->thread_context.uc_link = &running->thread_context; 
  makecontext(&uc->thread_context, fct, 1, id);
  t_queue(uc);
}

/* Terminate currently running thread */
int t_terminate()
{
  tcb *tmp;

  tmp = running;
  running = running->next;
  free(tmp);

  setcontext(&running->thread_context);
}

/* Move currently running thread to end of ready queue, start up next ready thread */
void t_yield()
{
  tcb *tmp;

  tmp = running;
  running = running->next;
  t_queue(tmp);

  swapcontext(&tmp->thread_context, &running->thread_context);
}