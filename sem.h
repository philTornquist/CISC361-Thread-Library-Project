#include "t_lib.h"

struct sem_t {
  int sem_count;
  tcb *block_queue;
  tcb *end_queue;
};

typedef struct sem_t sem_t;

int sem_init(sem_t **sp, int sem_count);
void sem_wait(sem_t *sp);
void sem_signal(sem_t *sp);
void sem_destroy(sem_t **sp);