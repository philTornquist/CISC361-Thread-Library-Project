#include "t_lib.h"
#include "sem.h"

int sem_init(sem_t **sp, int sem_count)
{
  *sp = malloc(sizeof(sem_t));
  (*sp)->sem_count = sem_count;
  (*sp)->block_queue = NULL;
}

void sem_wait(sem_t *sp)
{
  sighold(SIGALRM);
  sp->sem_count--;
  if (sp->sem_count < 0)
    t_block(&sp->block_queue, &sp->end_queue);
  else
    sigrelse(SIGALRM);
}

void sem_signal(sem_t *sp)
{
  sighold(SIGALRM);
  sp->sem_count++;
  if (sp->sem_count <= 0)
  {
    tcb *tmp = sp->block_queue;
    sp->block_queue = sp->block_queue->next;
    t_queue(tmp);
  }
  sigrelse(SIGALRM);
}

void sem_destroy(sem_t **sp)
{
  free(*sp);
}
