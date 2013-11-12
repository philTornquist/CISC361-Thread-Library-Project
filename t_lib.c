/*
 Phil Tornquist - 701526283
 Sean Moir      - 701411108
 CISC361-010
 t_lib.c
*/

#include "t_lib.h"
#include <signal.h>
#include <stdlib.h>

#define TIME_SLICE 100
#define LEVEL_2_QUEUE 
#define ROUND_ROBIN 

tcb *running;          //  Running thread and [running->next] is the start of the ready queue
tcb *end_queue;        //  End of ready queue
tcb *to_delete;

#ifdef LEVEL_2_QUEUE
tcb *end_level0;
#endif

/* 
 * sig_hand()
 */
void sig_hand(int signo)
{
  t_yield();
}

/*
 * t_queue()
 * Add thread to end of ready queue 
 */
void t_queue(tcb *thread)
{
#ifdef LEVEL_2_QUEUE
  
  if (thread->thread_priority == 0)
  {
    thread->next = end_level0->next;
    end_level0->next = thread;
    end_level0 = thread;

    if (thread->next == NULL) end_queue = end_level0;
  }
  else if (thread->thread_priority == 1)
  {
    end_queue->next = thread;
    end_queue = thread;
    end_queue->next = NULL;
  }

#else
  end_queue->next = thread;
  end_queue = thread;
  end_queue->next = NULL;
#endif
}

/* 
 * t_free()
 */
void t_free(tcb *t)
{
  tcb *thread = to_delete;
  to_delete = t;
  if (thread != NULL)
  {  
  free(thread->thread_context.uc_stack.ss_sp);
  free(thread);
  }
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

#ifdef LEVEL_2_QUEUE
  end_level0 = running;
#endif

#ifdef ROUND_ROBIN
  struct sigaction  act, oact;

  act.sa_handler = t_yield;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_flags |= SA_INTERRUPT;
  if (sigaction(SIGALRM, &act, &oact) < 0)
  {
    printf("Couldn't setup signal handler\n");
    exit(2);
  }
  ualarm(TIME_SLICE,0);
#endif
}

/* 
 * t_shutdown()
 * Shut down thread library 
 */
void t_shutdown()
{
#ifdef ROUND_ROBIN
  ualarm(0,0);
#endif
  tcb *tmp = running;
  running = running->next;
  free(tmp);
  while (running != NULL) {
    tmp = running;
    running = running->next;
    t_free(tmp);
  }
  t_free(NULL);
}

/*
 * start_thread()
 */
void start_thread(int id, void (*fct)(int))
{ 
#ifdef ROUND_ROBIN
  sigrelse(SIGALRM);
  ualarm(TIME_SLICE, 0);
#endif
  fct(id);

  //  If this is the last thread alive then terminating it
  //  then shutdown the thread library
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
  sighold(SIGALRM);
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
  sigrelse(SIGALRM);
}

/* 
 * t_terminate()
 * Terminate currently running thread 
 */
int t_terminate()
{
#ifdef ROUND_ROBIN
  sighold(SIGALRM);
  ualarm(0,0);
#endif
  if (running->next == NULL) return -1;

  tcb *tmp = running;
  running = running->next;

  t_free(tmp);
  
  setcontext(&running->thread_context);
}

/* 
 * t_yield()
 * Move currently running thread to end of ready queue, start up next ready thread 
 */
void t_yield()
{
#ifdef ROUND_ROBIN
  sighold(SIGALRM);
  ualarm(0, 0);
#endif
  tcb *tmp = running;
  running = running->next;

  if (running == NULL) {
    running = tmp;
    #ifdef LEVEL_2_QUEUE
    end_level0 = running;
    #endif
    end_queue = running;
    return;
  }

  t_queue(tmp);

  swapcontext(&tmp->thread_context, &running->thread_context);
#ifdef ROUND_ROBIN
  sigrelse(SIGALRM);
  ualarm(TIME_SLICE, 0);
#endif
}

/*
 * t_block()
 * Stops the current running thread and adds it to the given queue
 */
void t_block(tcb **queue_start, tcb **queue_end)
{
#ifdef ROUND_ROBIN
  sighold(SIGALRM);
  ualarm(0,0);
#endif

  if (running->next == NULL)
  {
    printf("DEADLOCK");
  }

  if (*queue_start == NULL)
  {
    *queue_start = running;
    *queue_end = running;
  }
  else
  {
    (*queue_end)->next = running;
    *queue_end = running;
  }

  tcb *tmp = running;
  running = running->next;
  tmp->next = NULL;

  swapcontext(&tmp->thread_context, &running->thread_context);
#ifdef ROUND_ROBIN
  sigrelse(SIGALRM);
  ualarm(TIME_SLICE, 0);
#endif
}
