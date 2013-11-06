/*
 Phil Tornquist
 Sean Moir
 CISC361-010
 t_lib.c
*/

#include "t_lib.h"
#include <signal.h>

#define LEVEL_2_QUEUE 1
#define ROUND_ROBIN 1

struct tcb {
  int         thread_id;
  int         thread_priority;
  ucontext_t  thread_context;
  struct tcb *next;
};

typedef struct tcb tcb;

tcb *running;
tcb *end_queue;

void sig_hand(int signo)
{
printf("signal\n");
ualarm(10,10);
t_yield();
}

#ifdef LEVEL_2_QUEUE
tcb *end_level0;
#endif

/* Add thread to end of ready queue */
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
  
  tcb* tmp = running;
  while (tmp != NULL)
  {
  	printf("%i,%i -> ", tmp->thread_id, tmp->thread_priority);
  	if (tmp == end_level0)
  		printf("end level 0");
  	printf("\n");
  	tmp = tmp->next;
  }
  printf("End Queue Post\n\n");

#else
  end_queue->next = thread;
  end_queue = thread;
  end_queue->next = NULL;
#endif
}

/* Initialize thread library */
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

  act.sa_handler = sig_hand;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_flags |= SA_INTERRUPT;
  if (sigaction(SIGALRM, &act, &oact) < 0)
  {
    printf("Couldn't setup signal handler\n");
    exit(2);
  }
  ualarm(10,10);
#endif
}

/* Shut down thread library */
void t_shutdown()
{
#ifdef ROUND_ROBIN
  signal(SIGALRM, SIG_DFL);
  ualarm(0,0);
#endif

  while (running != NULL) {
    tcb *tmp = running;
    running = running->next;
    free(tmp);
  }
}

void start_thread(int id, void (*fct)(int))
{
printf("thread created\n");
	sigrelse(SIGALRM);
	fct(id);
}

/* Create new thread */
int t_create(void (*fct)(void), int id, int pri)
{
  sighold(SIGALRM);
  //printf("\nthread: %i\n\n", id);
  size_t sz = 0x10000;

  tcb *uc;
  uc = (tcb *) malloc(sizeof(tcb));
  uc->thread_priority = pri;
  uc->thread_id = id;

  getcontext(&uc->thread_context);
  //uc->thread_context.uc_stack.ss_sp = mmap(0, sz,
  //     PROT_READ | PROT_WRITE | PROT_EXEC,
 //      MAP_PRIVATE | MAP_ANON, -1, 0);
  uc->thread_context.uc_stack.ss_sp = malloc(sz);
  uc->thread_context.uc_stack.ss_size = sz;
  uc->thread_context.uc_stack.ss_flags = 0;
  makecontext(&uc->thread_context, start_thread, 2, id, fct);
  t_queue(uc);
 
  sigrelse(SIGALRM);
}

/* Terminate currently running thread */
int t_terminate()
{
  sighold(SIGALRM);
  tcb *tmp;

  tmp = running;
  running = running->next;
  free(tmp);
  
  sigrelse(SIGALRM);
  setcontext(&running->thread_context);
}

/* Move currently running thread to end of ready queue, start up next ready thread */
void t_yield()
{
  sighold(SIGALRM);
  tcb *tmp;

  tmp = running;
  running = running->next;

  if (running == NULL) {
    running = tmp;
    end_level0 = running;
    end_queue = running;
    sigrelse(SIGALRM);
    return;
  }

#ifdef LEVEL_2_QUEUE
  //  level 0 queue is empty
  if (end_level0 == tmp)
    end_level0 = running;
#endif

  t_queue(tmp);

  swapcontext(&tmp->thread_context, &running->thread_context);
  sigrelse(SIGALRM);
}
