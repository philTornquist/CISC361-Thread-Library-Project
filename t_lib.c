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

void t_queue(tcb *thread)
{
  /* Add thread to end of ready queue */
  end_queue->next = thread;
  end_queue = thread;
  end_queue->next = NULL;
}

void t_init()
{
  tcb *tmp;
  tmp = (tcb *) malloc(sizeof(tcb));
  tmp->next = NULL;

  getcontext(&tmp->thread_context);    /* let tmp be the context of main() */
  running = tmp;

  end_queue = tmp;
}

void t_shutdown()
{
  while (running != NULL) {
    tcb *tmp = running;
    running = running->next;
    free(tmp);
  }
}

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

int t_terminate()
{
  tcb *tmp;

  tmp = running;
  running = running->next;
  free(tmp);

  setcontext(&running->thread_context);
}

void t_yield()
{
  tcb *tmp;

  tmp = running;
  running = running->next;
  t_queue(tmp);

  swapcontext(&tmp->thread_context, &running->thread_context);
}